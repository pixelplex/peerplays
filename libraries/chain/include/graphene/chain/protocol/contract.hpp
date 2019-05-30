#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene { namespace chain {


    struct wavm_op {

        account_id_type registrar;

        optional<contract_id_type> receiver;

        std::string action;

        std::string data;
    };

    struct contract_operation : public base_operation
    {
        struct fee_parameters_type{
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10; /// only required for large memos.
        };

        /// Fee for `contract_operation`
        asset               fee;
        /// Fee payer
        account_id_type     registrar;

        /// Type of VM, which will be run
        vm_types          vm_type;
        /// Serealized data for VM
        bytes   data;

        account_id_type fee_payer()const { return registrar; }
        void            validate()const;
        share_type      calculate_fee(const fee_parameters_type& )const;
    };

    struct contract_transfer_operation : public base_operation
    {
       struct fee_parameters_type {
          uint64_t fee       = 0;
       };
    
       asset                fee;
       /// Contract to transfer asset from
       contract_id_type     from;
       /// Account or contract to transfer asset to
       object_id_type       to;
       /// The amount of asset to transfer from @ref from to @ref to
       asset                amount;
    
       /// User provided data encrypted to the memo key of the "to" account
       extensions_type   extensions;
    
       account_id_type fee_payer()const { return account_id_type(); }
       void            validate()const;
       share_type      calculate_fee(const fee_parameters_type& k)const;
    };

} } // graphene::chain

FC_REFLECT( graphene::chain::wavm_op, (registrar)(receiver)(action)(data) )
FC_REFLECT( graphene::chain::contract_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::contract_operation, (fee)(registrar)(vm_type)(data) )

FC_REFLECT( graphene::chain::contract_transfer_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::contract_transfer_operation, (fee)(from)(to)(amount)(extensions) )
