// SPDX-License-Identifier: Apache-2.0

//
// Created by Keren Dong on 2020/3/12.
//

#ifndef WINGCHUN_BROKER_CLIENT_H
#define WINGCHUN_BROKER_CLIENT_H

#include <kungfu/longfist/longfist.h>
#include <kungfu/wingchun/broker/broker.h>
#include <kungfu/yijinjing/io.h>
#include <kungfu/yijinjing/log.h>
#include <kungfu/yijinjing/practice/apprentice.h>

namespace kungfu::wingchun::broker {
/**
 * Policy interface to decide the time point to resume when connecting to a broker.
 */
struct ResumePolicy {
  [[nodiscard]] virtual int64_t get_connect_time(const yijinjing::practice::apprentice &app,
                                                 const longfist::types::Register &broker) const;

  [[nodiscard]] virtual int64_t get_resume_time(const yijinjing::practice::apprentice &app,
                                                const longfist::types::Register &broker) const = 0;
};

/**
 * Always resume from the last unread frame, is intended to be used by system services that needs continuity.
 */
struct StatelessResumePolicy : public ResumePolicy {
  [[nodiscard]] int64_t get_resume_time(const yijinjing::practice::apprentice &app,
                                        const longfist::types::Register &broker) const override;
};

/**
 * Always resume from the last unread frame, is intended to be used by system services that needs continuity.
 */
struct ContinuousResumePolicy : public ResumePolicy {
  [[nodiscard]] int64_t get_resume_time(const yijinjing::practice::apprentice &app,
                                        const longfist::types::Register &broker) const override;
};

/**
 * Resumes from the last unread frame, or the start of today if the last unread frame was before it.
 * This policy ensures the client does not look back data before today, is intended to be used by strategies.
 */
struct IntradayResumePolicy : public ResumePolicy {
  [[nodiscard]] int64_t get_resume_time(const yijinjing::practice::apprentice &app,
                                        const longfist::types::Register &broker) const override;
};

struct FromNowResumePolicy : public ResumePolicy {
  [[nodiscard]] int64_t get_connect_time(const yijinjing::practice::apprentice &app,
                                         const longfist::types::Register &broker) const override;

  [[nodiscard]] int64_t get_resume_time(const yijinjing::practice::apprentice &app,
                                        const longfist::types::Register &broker) const override;
};

/**
 * Manage connections to brokers.
 */
class Client {
  typedef std::unordered_map<uint32_t, longfist::types::InstrumentKey> InstrumentKeyMap;
  typedef std::unordered_map<uint32_t, longfist::enums::BrokerState> BrokerStateMap;
  typedef std::unordered_map<std::string, yijinjing::data::location_ptr> ExchangeSourceMap;
  typedef std::unordered_map<uint32_t, yijinjing::data::location_ptr> InstrumentSourceMap;

public:
  explicit Client(yijinjing::practice::apprentice &app);

  virtual ~Client() = default;

  [[nodiscard]] virtual const ResumePolicy &get_resume_policy() const = 0;

  [[nodiscard]] const InstrumentKeyMap &get_instrument_keys() const;

  [[nodiscard]] virtual bool is_ready(uint32_t broker_location_uid) const;

  [[nodiscard]] virtual bool is_connected(uint32_t broker_location_uid) const;

  [[nodiscard]] virtual bool is_custom_subscribed(uint32_t md_location_uid) const = 0;

  [[nodiscard]] virtual bool is_custom_subscribed_all(uint32_t md_location_uid,
                                                      kungfu::longfist::enums::SubscribeDataType data_type,
                                                      const std::string &exchange_id,
                                                      longfist::enums::InstrumentType kf_instrument_type) const = 0;

  [[nodiscard]] virtual bool is_all_subscribed(uint32_t md_location_uid) const = 0;

  [[nodiscard]] virtual bool is_subscribed(const std::string &exchange_id, const std::string &instrument_id) const;

  virtual void subscribe(const longfist::types::InstrumentKey &instrument_key);

  virtual void subscribe(const std::string &exchange_id, const std::string &instrument_id);

  virtual void subscribe(const yijinjing::data::location_ptr &md_location, const std::string &exchange_id,
                         const std::string &instrument_id);

  virtual void connect(const event_ptr &event, const longfist::types::Register &register_data);

  virtual void connect(const event_ptr &event, const longfist::types::Band &band);

  virtual void renew(int64_t trigger_time, const yijinjing::data::location_ptr &md_location);

  virtual bool try_renew(int64_t trigger_time, const yijinjing::data::location_ptr &md_location);

  virtual void sync(int64_t trigger_time, const yijinjing::data::location_ptr &td_location);

  virtual bool try_sync(int64_t trigger_time, const yijinjing::data::location_ptr &td_location);

  virtual void on_start(const rx::connectable_observable<event_ptr> &events);

  [[nodiscard]] virtual bool should_connect_md(const yijinjing::data::location_ptr &md_location) const = 0;

  [[nodiscard]] virtual bool should_connect_td(const yijinjing::data::location_ptr &md_location) const = 0;

  [[nodiscard]] virtual bool should_connect_md(uint32_t md_location_uid) const = 0;

  [[nodiscard]] virtual bool should_connect_td(uint32_t td_location_uid) const = 0;

  [[nodiscard]] virtual bool should_connect_strategy(const yijinjing::data::location_ptr &md_location) const = 0;

  kungfu::yijinjing::data::location_ptr get_location(uint32_t uid) const { return app_.get_location(uid); }

protected:
  yijinjing::practice::apprentice &app_;

private:
  BrokerStateMap broker_states_ = {};
  InstrumentKeyMap instrument_keys_ = {};
  ExchangeSourceMap exchange_md_locations_ = {};
  InstrumentSourceMap instrument_md_locations_ = {};
  yijinjing::data::location_map ready_md_locations_ = {};
  yijinjing::data::location_map ready_td_locations_ = {};

  void update_broker_state(const event_ptr &event, const longfist::types::BrokerStateUpdate &state);

  void update_broker_state(const event_ptr &event, const longfist::types::Deregister &deregister_data);
};

/**
 * Automatically connects all brokers, and subscribe only the instruments that has been explicitly added.
 * In addition to brokers it also handle connections to strategies. This is intended to be used by system services like
 * ledger, risk, watcher, etc.
 */
class AutoClient : public Client {
public:
  explicit AutoClient(yijinjing::practice::apprentice &app);

  [[nodiscard]] const ResumePolicy &get_resume_policy() const override;

  [[nodiscard]] bool is_custom_subscribed(uint32_t md_location_uid) const override;

  [[nodiscard]] bool is_custom_subscribed_all(uint32_t md_location_uid,
                                              kungfu::longfist::enums::SubscribeDataType data_type,
                                              const std::string &exchange_id,
                                              longfist::enums::InstrumentType kf_instrument_type) const override;

  [[nodiscard]] bool is_all_subscribed(uint32_t md_location) const override;

protected:
  [[nodiscard]] bool should_connect_md(const yijinjing::data::location_ptr &md_location) const override;

  [[nodiscard]] bool should_connect_td(const yijinjing::data::location_ptr &md_location) const override;

  [[nodiscard]] bool should_connect_md(uint32_t md_location_uid) const override;

  [[nodiscard]] bool should_connect_td(uint32_t td_location_uid) const override;

  [[nodiscard]] bool should_connect_strategy(const yijinjing::data::location_ptr &md_location) const override;

private:
  StatelessResumePolicy resume_policy_ = {};
};

/**
 * Automatically connect all brokers and strategies.
 * It differs from AutoClient in the sense that it does not issue any subscriptions, but assumes all instruments that
 * seen is subscribed.
 */
class SilentAutoClient : public AutoClient {
public:
  explicit SilentAutoClient(yijinjing::practice::apprentice &app);

  // [[nodiscard]] bool is_subscribed(const std::string &exchange_id, const std::string &instrument_id) const override;

  void renew(int64_t trigger_time, const yijinjing::data::location_ptr &md_location) override;

  void sync(int64_t trigger_time, const yijinjing::data::location_ptr &td_location) override;
};

/**
 * Only connects brokers that has been explicitly added. It supports subscribe_all for MD that has such ability.
 */
class PassiveClient : public Client {
  typedef std::unordered_map<uint32_t, bool> EnrollmentMap;
  typedef std::unordered_map<uint32_t, std::vector<longfist::types::CustomSubscribe>> CustomSubscribeMap;

public:
  explicit PassiveClient(yijinjing::practice::apprentice &app);

  [[nodiscard]] const ResumePolicy &get_resume_policy() const override;

  [[nodiscard]] bool is_custom_subscribed(uint32_t md_location_uid) const override;

  [[nodiscard]] bool is_custom_subscribed_all(uint32_t md_location_uid,
                                              kungfu::longfist::enums::SubscribeDataType data_type,
                                              const std::string &exchange_id,
                                              longfist::enums::InstrumentType kf_instrument_type) const override;

  [[nodiscard]] bool is_all_subscribed(uint32_t md_location) const override;

  void subscribe(const yijinjing::data::location_ptr &md_location, const std::string &exchange_id,
                 const std::string &instrument_id) override;

  void subscribe_all(const yijinjing::data::location_ptr &md_location, uint8_t market_type = 0,
                     uint64_t instrument_type = 0, uint64_t data_type = 0);

  void renew(int64_t trigger_time, const yijinjing::data::location_ptr &md_location) override;

  void sync(int64_t trigger_time, const yijinjing::data::location_ptr &td_location) override;

  void enroll_account(const yijinjing::data::location_ptr &td_location);

protected:
  [[nodiscard]] bool should_connect_md(const yijinjing::data::location_ptr &md_location) const override;

  [[nodiscard]] bool should_connect_td(const yijinjing::data::location_ptr &md_location) const override;

  [[nodiscard]] bool should_connect_md(uint32_t md_location_uid) const override;

  [[nodiscard]] bool should_connect_td(uint32_t td_location_uid) const override;

  [[nodiscard]] bool should_connect_strategy(const yijinjing::data::location_ptr &md_location) const override;

private:
  FromNowResumePolicy resume_policy_ = {};
  CustomSubscribeMap custom_subs_ = {};
  EnrollmentMap enrolled_md_locations_ = {};
  EnrollmentMap enrolled_td_locations_ = {};
};

template <typename DataType>
static constexpr auto is_md_datatype_v =
    std::is_same_v<DataType, longfist::types::Quote> or std::is_same_v<DataType, longfist::types::Entrust> or
    std::is_same_v<DataType, longfist::types::Transaction> or std::is_same_v<DataType, longfist::types::Tree>;

template <typename DataType, std::enable_if_t<is_md_datatype_v<DataType>>...>
static constexpr auto is_own(const Client &broker_client) {
  return rx::filter([&](const event_ptr &event) {
    if (event->msg_type() == DataType::tag) {
      const DataType &data = event->data<DataType>();
      if (broker_client.is_custom_subscribed(event->source())) {
        if ((std::is_same_v<DataType, longfist::types::Quote> &&
             broker_client.is_custom_subscribed_all(event->source(),
                                                    kungfu::longfist::enums::SubscribeDataType::Snapshot,
                                                    data.exchange_id, data.instrument_type)) ||
            (std::is_same_v<DataType, longfist::types::Tree> &&
             broker_client.is_custom_subscribed_all(event->source(),
                                                    kungfu::longfist::enums::SubscribeDataType::Snapshot,
                                                    data.exchange_id, data.instrument_type)) ||
            (std::is_same_v<DataType, longfist::types::Transaction> &&
             broker_client.is_custom_subscribed_all(event->source(),
                                                    kungfu::longfist::enums::SubscribeDataType::Transaction,
                                                    data.exchange_id, data.instrument_type)) ||
            (std::is_same_v<DataType, longfist::types::Entrust> &&
             broker_client.is_custom_subscribed_all(event->source(),
                                                    kungfu::longfist::enums::SubscribeDataType::Entrust,
                                                    data.exchange_id, data.instrument_type))) {
          return true;
        }
      }
      if (broker_client.is_subscribed(data.exchange_id, data.instrument_id)) {
        return true;
      }
    }
    return false;
  });
};

template <typename DataType, std::enable_if_t<std::is_same_v<DataType, longfist::types::Register> or
                                              std::is_same_v<DataType, longfist::types::Deregister>>...>
static constexpr auto is_own(const Client &broker_client) {
  return rx::filter([&](const event_ptr &event) {
    if (event->msg_type() == DataType::tag) {
      const DataType &data = event->data<DataType>();
      return broker_client.should_connect_md(data.location_uid) or broker_client.should_connect_td(data.location_uid);
    }
    return false;
  });
};

template <typename DataType, std::enable_if_t<std::is_same_v<DataType, longfist::types::BrokerStateUpdate>>...>
static constexpr auto is_own(const Client &broker_client) {
  return rx::filter([&](const event_ptr &event) {
    if (event->msg_type() == DataType::tag) {
      const DataType &data = event->data<DataType>();
      return (broker_client.should_connect_md(event->source()) or broker_client.should_connect_td(event->source()));
    }
    return false;
  });
};

} // namespace kungfu::wingchun::broker

#endif // WINGCHUN_BROKER_CLIENT_H
