#pragma once
#include <fc/uint128.hpp>

#include <graphene/db/object.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene { namespace chain {

class period_object : public abstract_object<period_object>
{
   public:
      static const uint8_t space_id = implementation_ids;
      static const uint8_t type_id = impl_period_object_type;

      share_type            whole_period_budget;
      share_type            witness_pool;
      share_type            current_supply;
      time_point_sec        end_time;

};
} }

FC_REFLECT_DERIVED( graphene::chain::period_object, (graphene::db::object),
                    (whole_period_budget)
                    (witness_pool)
                    (current_supply)
                    (end_time)
                  )