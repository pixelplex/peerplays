#pragma once
#include <graphene/chain/protocol/types.hpp>


namespace btc_sidechain {

   struct btc_sidechain_parameters_extension {
      uint8_t  maximum_condensing_tx_vins          = BTC_SIDECHAIN_DEFAULT_MAX_CONDENSING_TX_VINS;
      uint8_t  maximum_condensing_tx_vouts         = BTC_SIDECHAIN_DEFAULT_MAX_CONDENSING_TX_VOUTS;
      uint8_t  maximum_unconfirmed_vouts           = BTC_SIDECHAIN_DEFAULT_MAX_UNCONFIRMED_VOUTS;
      uint16_t percent_payment_to_witnesses        = BTC_SIDECHAIN_DEFAULT_PERCENT_PAYMENT_TO_WITNESSES;
      uint8_t  multisig_sigs_num                   = BTC_SIDECHAIN_DEFAULT_NUMBER_SIG_MULTISIG;
      uint8_t  confirmations_num                   = BTC_SIDECHAIN_DEFAULT_NUMBER_OF_CONFIRMATIONS;

      graphene::chain::account_id_type managing_account;
      graphene::chain::asset_id_type asset_id;
   };

} // namespace btc_sidechain

FC_REFLECT( btc_sidechain::btc_sidechain_parameters_extension, 
            (maximum_condensing_tx_vins)
            (maximum_condensing_tx_vouts)
            (maximum_unconfirmed_vouts)
            (percent_payment_to_witnesses)
            (multisig_sigs_num)
            (confirmations_num)
            (managing_account)
            (asset_id)
)