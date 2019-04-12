#pragma once
#include <graphene/chain/database.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
using namespace graphene::chain;

namespace vms { namespace base {

/**
 * @class fee_gas
 * @brief Manager class for fee
 */
class fee_gas
{
    public:
    fee_gas(database& db, transaction_evaluation_state& eval_state): d(db), trx_state(&eval_state) {}

    /**
     * @brief Full cycle of fee processing
     * 
     * @param fee amount of fee
     * @param op operation for fee
     */
    void process_fee(share_type fee, const contract_operation& op);
    /**
     * @brief Calculate fee for this operation (according to core asset)
     * 
     * @param fee amount of fee
     * @param op operation for fee
     */
    void prepare_fee(share_type fee, const contract_operation& op);

    private:
    /// Convert fee in fee dynamic data
    void convert_fee();
    /// Pay fee for operation
    void pay_fee();

    database&                        d;
    asset                            fee_from_account;
    share_type                       core_fee_paid;
    const account_object*            fee_paying_account = nullptr;
    const account_statistics_object* fee_paying_account_statistics = nullptr;
    const asset_object*              fee_asset          = nullptr;
    const asset_dynamic_data_object* fee_asset_dyn_data = nullptr;
    transaction_evaluation_state*    trx_state;
};

} }
