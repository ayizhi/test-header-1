// SPDX-License-Identifier: Apache-2.0

//
// Created by Keren Dong on 2020/4/6.
//

#ifndef WINGCHUN_BOOKKEEPER_H
#define WINGCHUN_BOOKKEEPER_H

#include <kungfu/wingchun/book/accounting.h>
#include <kungfu/wingchun/broker/client.h>
#include <kungfu/yijinjing/practice/apprentice.h>

namespace kungfu::wingchun::book {
// key = location_uid
typedef std::unordered_map<uint32_t, Book_ptr> BookMap;

typedef std::unordered_map<longfist::enums::InstrumentType, AccountingMethod_ptr> AccountingMethodMap;

FORWARD_DECLARE_CLASS_PTR(Context)
class BookListener {
public:
  virtual void on_position_sync_reset(const Book &old_book, const Book &new_book){};
  virtual void on_asset_sync_reset(const longfist::types::Asset &old_asset, const longfist::types::Asset &new_asset){};
  virtual void on_asset_margin_sync_reset(const longfist::types::AssetMargin &old_asset_margin,
                                          const longfist::types::AssetMargin &new_asset_margin){};
};
DECLARE_PTR(BookListener)

class Bookkeeper {
public:
  explicit Bookkeeper(yijinjing::practice::apprentice &app, broker::Client &broker_client);

  virtual ~Bookkeeper() = default;

  bool has_book(uint32_t location_uid);

  Book_ptr get_book(uint32_t uid);

  void drop_book(uint32_t uid);

  [[nodiscard]] const BookMap &get_books() const;

  void set_accounting_method(longfist::enums::InstrumentType instrument_type,
                             const AccountingMethod_ptr &accounting_method);

  void on_trading_day(int64_t daytime);

  void on_start(const rx::connectable_observable<event_ptr> &events);

  void on_order_input(int64_t update_time, uint32_t source, uint32_t dest, const longfist::types::OrderInput &input);

  void restore(const yijinjing::cache::bank &state_bank);

  void guard_positions();

  void update_book(const event_ptr &event, const longfist::types::InstrumentKey &instrument_key);

  void update_book(const event_ptr &event, const longfist::types::Quote &quote);

  void add_book_listener(const BookListener_ptr &book_listener);

  void mirror_positions(int64_t trigger_time, uint32_t strategy_uid);

  void try_update_position_end(const longfist::types::PositionEnd &position_end);

  template <typename TradingData, typename ApplyMethod = void (AccountingMethod::*)(Book_ptr, const TradingData &)>
  void update_book(const event_ptr &event, ApplyMethod method) {
    update_book(event->gen_time(), event->source(), event->dest(), event->data<TradingData>(), method);
  }

  template <typename TradingData, typename ApplyMethod = void (AccountingMethod::*)(Book_ptr, const TradingData &)>
  void update_book(int64_t update_time, uint32_t source, uint32_t dest, const TradingData &data, ApplyMethod method) {
    std::lock_guard<std::mutex> lock(update_book_mutex_);

    if (accounting_methods_.find(data.instrument_type) == accounting_methods_.end()) {
      SPDLOG_WARN("accounting method not found for {}: {}", data.type_name.c_str(), data.to_string());
      return;
    }
    AccountingMethod &accounting_method = *accounting_methods_.at(data.instrument_type);
    auto apply_and_update = [&](uint32_t book_uid) {
      auto book = get_book(book_uid);
      auto &position = book->get_position_for(data);
      (accounting_method.*method)(book, data);
      position.update_time = update_time;
      book->replace(data);
      book->update(update_time);
    };
    apply_and_update(source);
    if (dest != yijinjing::data::location::PUBLIC) {
      apply_and_update(dest);
    }
  }

  /// 根据event->dest() == dest 选择触发t1还是t2函数
  template <typename T, typename RouteA = void (Bookkeeper::*)(const T &),
            typename RouteB = void (Bookkeeper::*)(const T &)>
  constexpr decltype(auto) fork(uint32_t dest, RouteA t1, RouteB t2) {
    return kungfu::rx::$([&, dest, t1, t2](const event_ptr &event) {
      if (event->msg_type() != T::tag) {
        return;
      }
      if (event->dest() == dest) {
        auto &data = event->data<T>();
        (this->*t1)(data);
      } else {
        auto &data = event->data<T>();
        (this->*t2)(data);
      }
    });
  }

private:
  yijinjing::practice::apprentice &app_;
  broker::Client &broker_client_;

  std::mutex update_book_mutex_;
  bool positions_guarded_ = false;
  CommissionMap commissions_ = {};
  InstrumentMap instruments_ = {};
  BookMap books_ = {};
  AccountingMethodMap accounting_methods_ = {};
  std::vector<BookListener_ptr> book_listeners_ = {};
  BookMap books_replica_ = {}; // 暂存从location::SYNC传来的asset和position信息
  std::unordered_map<uint32_t, bool> books_replica_asset_guards_ = {}; // Asset::tag添加对应<location_uid,true>
  std::unordered_map<uint32_t, bool> books_replica_asset_margin_guards_ = {}; // AssetMargin::tag-><location_uid,true>
  std::unordered_map<uint32_t, bool> books_replica_position_guard_ = {}; // PositionEnd::tag添加对应<location_uid,true>

  Book_ptr make_book(uint32_t location_uid);

  void update_instrument(const longfist::types::Instrument &instrument);

  void try_update_asset(const longfist::types::Asset &asset);

  void try_update_asset_margin(const longfist::types::AssetMargin &asset_margin);

  void try_update_position(const longfist::types::Position &position);

  /// 把books_replica_中location_uid对应的book复制到books_，然后重置asset_guards和position_guards为false
  void try_sync_book_replica(uint32_t location_uid);

  void try_update_asset_replica(const longfist::types::Asset &asset);

  void try_update_assetmargin_replica(const longfist::types::AssetMargin &asset_margin);

  void try_update_position_replica(const longfist::types::Position &position);

  void update_position_guard(const longfist::types::PositionEnd &position_end);

  Book_ptr get_book_replica(uint32_t location_uid);
};
} // namespace kungfu::wingchun::book
#endif // WINGCHUN_BOOKKEEPER_H