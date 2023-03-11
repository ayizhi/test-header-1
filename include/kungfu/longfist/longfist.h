// SPDX-License-Identifier: Apache-2.0

//
// Created by Keren Dong on 2020/1/26.
//

#ifndef KUNGFU_LONGFIST_H
#define KUNGFU_LONGFIST_H

#include "kungfu/yijinjing/cache/ringqueue.h"
#include <kungfu/longfist/types.h>

#define TYPE_PAIR(DataType) boost::hana::make_pair(HANA_STR(#DataType), boost::hana::type_c<types::DataType>)

namespace kungfu::longfist {
constexpr auto AllTypes = boost::hana::make_map( //
    TYPE_PAIR(PageEnd),                          //
    TYPE_PAIR(SessionStart),                     //
    TYPE_PAIR(SessionEnd),                       //
    TYPE_PAIR(Time),                             //
    TYPE_PAIR(Ping),                             //
    TYPE_PAIR(Pong),                             //
    TYPE_PAIR(RequestStop),                      //
    TYPE_PAIR(RequestStart),                     //
    TYPE_PAIR(RequestCached),                    //
    TYPE_PAIR(CachedReadyToRead),                //
    TYPE_PAIR(RequestCachedDone),                //
    TYPE_PAIR(CustomSubscribe),                  //
    TYPE_PAIR(NewOrderSingle),                   //
    TYPE_PAIR(CancelOrder),                      //
    TYPE_PAIR(CancelAllOrder),                   //
    TYPE_PAIR(BrokerStateRequest),               //
    TYPE_PAIR(ResetBookRequest),                 //
    TYPE_PAIR(MirrorPositionsRequest),           //
    TYPE_PAIR(AssetRequest),                     //
    TYPE_PAIR(PositionRequest),                  //
    TYPE_PAIR(KeepPositionsRequest),             //
    TYPE_PAIR(RebuildPositionsRequest),          //
    TYPE_PAIR(AlgoOrderInput),                   //
    TYPE_PAIR(AlgoOrderReport),                  //
    TYPE_PAIR(AlgoOrderModify),                  //
    TYPE_PAIR(Config),                           //
    TYPE_PAIR(RiskSetting),                      //
    TYPE_PAIR(TimeValue),                        //
    TYPE_PAIR(TimeKeyValue),                     //
    TYPE_PAIR(StrategyStateUpdate),              //
    TYPE_PAIR(Commission),                       //
    TYPE_PAIR(Session),                          //
    TYPE_PAIR(Location),                         //
    TYPE_PAIR(Register),                         //
    TYPE_PAIR(Deregister),                       //
    TYPE_PAIR(CacheReset),                       //
    TYPE_PAIR(BrokerStateUpdate),                //
    TYPE_PAIR(RequestReadFrom),                  //
    TYPE_PAIR(RequestReadFromPublic),            //
    TYPE_PAIR(RequestReadFromSync),              //
    TYPE_PAIR(RequestWriteTo),                   //
    TYPE_PAIR(RequestWriteToBand),               //
    TYPE_PAIR(Band),                             //
    TYPE_PAIR(Basket),                           //
    TYPE_PAIR(BasketInstrument),                 //
    TYPE_PAIR(TradingDay),                       //
    TYPE_PAIR(Channel),                          //
    TYPE_PAIR(ChannelRequest),                   //
    TYPE_PAIR(TimeRequest),                      //
    TYPE_PAIR(TimeReset),                        //
    TYPE_PAIR(Instrument),                       //
    TYPE_PAIR(InstrumentKey),                    //
    TYPE_PAIR(Quote),                            //
    TYPE_PAIR(Tree),                             //
    TYPE_PAIR(Entrust),                          //
    TYPE_PAIR(Transaction),                      //
    TYPE_PAIR(Bar),                              //
    TYPE_PAIR(BlockMessage),                     //
    TYPE_PAIR(OrderInput),                       //
    TYPE_PAIR(OrderAction),                      //
    TYPE_PAIR(OrderActionError),                 //
    TYPE_PAIR(Order),                            //
    TYPE_PAIR(Trade),                            //
    TYPE_PAIR(HistoryOrder),                     //
    TYPE_PAIR(HistoryTrade),                     //
    TYPE_PAIR(Asset),                            //
    TYPE_PAIR(AssetMargin),                      //
    TYPE_PAIR(Position),                         //
    TYPE_PAIR(PositionEnd),                      //
    TYPE_PAIR(OrderStat),                        //
    TYPE_PAIR(BasketOrder),                      //
    TYPE_PAIR(RequestHistoryOrder),              //
    TYPE_PAIR(RequestHistoryOrderError),         //
    TYPE_PAIR(RequestHistoryTrade),              //
    TYPE_PAIR(RequestHistoryTradeError)          //
);

constexpr auto AllDataTypes = boost::hana::make_map( //
    TYPE_PAIR(Config),                               //
    TYPE_PAIR(RiskSetting),                          //
    TYPE_PAIR(TimeValue),                            //
    TYPE_PAIR(TimeKeyValue),                         //
    TYPE_PAIR(StrategyStateUpdate),                  //
    TYPE_PAIR(Commission),                           //
    TYPE_PAIR(Session),                              //
    TYPE_PAIR(Location),                             //
    TYPE_PAIR(Register),                             //
    TYPE_PAIR(Deregister),                           //
    TYPE_PAIR(CacheReset),                           //
    TYPE_PAIR(BrokerStateUpdate),                    //
    TYPE_PAIR(RequestReadFrom),                      //
    TYPE_PAIR(RequestReadFromPublic),                //
    TYPE_PAIR(RequestReadFromSync),                  //
    TYPE_PAIR(RequestWriteTo),                       //
    TYPE_PAIR(RequestWriteToBand),                   //
    TYPE_PAIR(Band),                                 //
    TYPE_PAIR(Basket),                               //
    TYPE_PAIR(BasketInstrument),                     //
    TYPE_PAIR(TradingDay),                           //
    TYPE_PAIR(Channel),                              //
    TYPE_PAIR(ChannelRequest),                       //
    TYPE_PAIR(TimeRequest),                          //
    TYPE_PAIR(TimeReset),                            //
    TYPE_PAIR(Instrument),                           //
    TYPE_PAIR(InstrumentKey),                        //
    TYPE_PAIR(Quote),                                //
    TYPE_PAIR(Tree),                                 //
    TYPE_PAIR(Entrust),                              //
    TYPE_PAIR(Transaction),                          //
    TYPE_PAIR(Bar),                                  //
    TYPE_PAIR(BlockMessage),                         //
    TYPE_PAIR(OrderInput),                           //
    TYPE_PAIR(OrderAction),                          //
    TYPE_PAIR(OrderActionError),                     //
    TYPE_PAIR(Order),                                //
    TYPE_PAIR(Trade),                                //
    TYPE_PAIR(HistoryOrder),                         //
    TYPE_PAIR(HistoryTrade),                         //
    TYPE_PAIR(Asset),                                //
    TYPE_PAIR(AssetMargin),                          //
    TYPE_PAIR(Position),                             //
    TYPE_PAIR(PositionEnd),                          //
    TYPE_PAIR(OrderStat),                            //
    TYPE_PAIR(BasketOrder)                           //
);

constexpr auto ProfileDataTypes = boost::hana::make_map( //
    TYPE_PAIR(Config),                                   //
    TYPE_PAIR(RiskSetting),                              //
    TYPE_PAIR(Commission),                               //
    TYPE_PAIR(Instrument),                               //
    TYPE_PAIR(Location),                                 //
    TYPE_PAIR(Basket),                                   //
    TYPE_PAIR(BasketInstrument)                          //
);

constexpr auto SessionDataTypes = boost::hana::make_map( //
    TYPE_PAIR(Session)                                   //
);

constexpr auto StateDataTypes = boost::hana::make_map( //
    TYPE_PAIR(Config),                                 //
    TYPE_PAIR(RiskSetting),                            //
    TYPE_PAIR(TimeValue),                              //
    TYPE_PAIR(TimeKeyValue),                           //
    TYPE_PAIR(StrategyStateUpdate),                    //
    TYPE_PAIR(Commission),                             //
    TYPE_PAIR(Instrument),                             //
    TYPE_PAIR(Basket),                                 //
    TYPE_PAIR(BasketInstrument),                       //
    TYPE_PAIR(Quote),                                  //
    TYPE_PAIR(Tree),                                   //
    TYPE_PAIR(OrderAction),                            //
    TYPE_PAIR(BlockMessage),                           //
    TYPE_PAIR(OrderInput),                             //
    TYPE_PAIR(Order),                                  //
    TYPE_PAIR(Trade),                                  //
    TYPE_PAIR(Asset),                                  //
    TYPE_PAIR(AssetMargin),                            //
    TYPE_PAIR(Position),                               //
    TYPE_PAIR(OrderStat),                              //
    TYPE_PAIR(BasketOrder)                             //
);

constexpr auto TradingDataTypes = boost::hana::make_map( //
    TYPE_PAIR(BlockMessage),                             //
    TYPE_PAIR(OrderInput),                               //
    TYPE_PAIR(Order),                                    //
    TYPE_PAIR(Trade),                                    //
    TYPE_PAIR(OrderStat),                                //
    TYPE_PAIR(BasketOrder)                               //
);

constexpr auto build_data_map = [](auto types) {
  auto maps = boost::hana::transform(boost::hana::values(types), [](auto value) {
    using DataType = typename decltype(+value)::type;
    return boost::hana::make_pair(value, std::unordered_map<uint64_t, DataType>());
  });
  return boost::hana::unpack(maps, boost::hana::make_map);
};

constexpr auto build_state_map = [](auto types) {
  auto maps = boost::hana::transform(boost::hana::values(types), [](auto value) {
    using DataType = typename decltype(+value)::type;
    return boost::hana::make_pair(value, std::unordered_map<uint64_t, state<DataType>>());
  });
  return boost::hana::unpack(maps, boost::hana::make_map);
};

constexpr auto build_ring_state_map = [](auto types, size_t ring_size) {
  auto maps = boost::hana::transform(boost::hana::values(types), [ring_size](auto value) {
    using DataType = typename decltype(+value)::type;
    kungfu::yijinjing::cache::ringqueue<state<DataType>> *p =
        new kungfu::yijinjing::cache::ringqueue<state<DataType>>(ring_size);
    return boost::hana::make_pair(value, p);
  });
  return boost::hana::unpack(maps, boost::hana::make_map);
};

using ProfileMapType = decltype(build_data_map(longfist::ProfileDataTypes));
DECLARE_PTR(ProfileMapType)

using StateMapType = decltype(build_state_map(longfist::StateDataTypes));
DECLARE_PTR(StateMapType)

static size_t TRADING_MAP_RING_SIZE = 2048;
using TradingMapType = decltype(build_ring_state_map(longfist::TradingDataTypes, TRADING_MAP_RING_SIZE));
DECLARE_PTR(TradingMapType)

template <typename DataType> std::enable_if_t<size_fixed_v<DataType>> copy(DataType &to, const DataType &from) {
  memcpy(&to, &from, sizeof(DataType));
}

template <typename DataType> std::enable_if_t<not size_fixed_v<DataType>> copy(DataType &to, const DataType &from) {
  boost::hana::for_each(boost::hana::accessors<DataType>(), [&](auto it) {
    auto accessor = boost::hana::second(it);
    accessor(to) = accessor(from);
  });
}
}; // namespace kungfu::longfist

#endif // KUNGFU_LONGFIST_H
