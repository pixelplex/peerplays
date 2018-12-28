#include <evm.hpp>
#include <fc/crypto/hex.hpp>

#include <aleth/libethcore/SealEngine.h>
#include <aleth/libethereum/ChainParams.h>
#include <aleth/libethashseal/GenesisInfo.h>

namespace vms { namespace evm {

bytes evm::exec( const bytes& data, const bool commit )
{
   eth_op eth = fc::raw::unpack<eth_op>( data );

   OnOpFunc const& _onOp = OnOpFunc();
   EnvInfo ei = create_environment();
   state.setAuthor( ei.author() );
   state.setAssetType( eth.asset_id.instance.value );
   state.clearResultAccount();

   Transaction tx = create_eth_transaction( eth );
   // std::unique_ptr< SealEngineFace > se( dev::eth::ChainParams(dev::eth::genesisInfo(dev::eth::Network::ExeBlockNetwork)).createSealEngine() );
   std::unique_ptr< SealEngineFace > se( dev::eth::ChainParams(dev::eth::genesisInfo(dev::eth::Network::FrontierTest)).createSealEngine() );
   auto permanence = get_adapter().evaluating_from_apply_block() ? Permanence::Committed : Permanence::Reverted;
   std::pair< ExecutionResult, TransactionReceipt > res = state.execute(ei, *se.get(), tx, permanence, _onOp);
   state.db().commit();
   state.publishContractTransfers();

   account_id_type author( address_to_id( ei.author() ).second );
   // db.adjust_balance(author, asset(-share_type(state.getFee()), asset_id_type( state.getAssetType() )));
   get_adapter().sub_account_balance( author.instance.value, state.getAssetType(), static_cast<int64_t>( state.getFee() ) );

   if(res.first.excepted != TransactionException::None){
      se->suicideTransfer.clear();
   }

   std::unordered_map<Address, Account> attracted_contr_and_acc = state.getResultAccounts();
   transfer_suicide_balances(se->suicideTransfer);
   delete_balances( attracted_contr_and_acc );
   // attracted_contracts = select_attracted_contracts(attracted_contr_and_acc);

   // bring in vm_executor

   // if( eval_state ) {
   //    if(db._evaluating_from_apply_block){
   //       state->addResult(db.get_index<result_contract_object>().get_next_id(), res);
   //       state->commitResults();
   //       state->dbResults().commit();
   //    }
   //    fee_gas fee(db, *eval_state);
   //    fee.process_fee(state->getFee(), c_o);
   // }

   // return res;
   return bytes();
}

Transaction evm::create_eth_transaction(const eth_op& eth) const
{   
   
   bytes code( eth.code.size() / 2, 0 );
   fc::from_hex( eth.code, code.data(), code.size() );

   Transaction tx;
   if( eth.receiver.valid() ) {
       dev::Address rec = id_to_address( eth.receiver->instance.value, 1 );
       tx = Transaction ( u256( eth.value ), u256( eth.gasPrice ), u256( eth.gas ), rec, dev::bytes(code.begin(), code.end()), u256( 0 ) );
   } else {
       tx = Transaction ( u256( eth.value ), u256( eth.gasPrice ), u256( eth.gas ), dev::bytes(code.begin(), code.end()), u256( 0 ) );
   }

    Address sender = id_to_address( eth.registrar.instance.value, 0 );
    tx.forceSender( sender );
    tx.setIdAsset( static_cast<uint64_t>( eth.asset_id.instance.value ) );

   return tx;
}

EnvInfo evm::create_environment()
{
   BlockHeader header;
   header.setNumber( static_cast<int64_t>( get_adapter().head_block_num() ) );
   header.setAuthor( id_to_address( get_adapter().get_id_author_block(), 0 ) );
   header.setTimestamp( static_cast<int64_t>( get_adapter().head_block_time() ) );
   header.setGasLimit( 1 << 30 );
   EnvInfo result(header, last_block_hashes(), u256());

   return result;
};

void evm::transfer_suicide_balances(const std::vector< std::pair< Address, Address > >& suicide_transfer) {
   for(std::pair< Address, Address > transfer : suicide_transfer){
      contract_id_type from( address_to_id( transfer.first ).second );

      auto to_temp = address_to_id( transfer.second );
      object_id_type to;
      if( to_temp.first ) {
         to = contract_id_type( to_temp.second );
      } else {
         to = account_id_type( to_temp.second );
      }

      auto balances = get_adapter().get_contract_balances( from.instance.value );
      for( auto& balance : balances ) {
         get_adapter().publish_contract_transfer( std::make_pair( 1, from.instance.value ),
                                                  std::make_pair( to_temp.first, to_temp.second ),
                                                  balance.second, balance.first );

         get_adapter().sub_contract_balance( from.instance.value, balance.second, balance.first );
         if ( to.is< account_id_type >() ) {
            get_adapter().add_account_balance( to.instance(), balance.second, balance.first );
         } else {
            get_adapter().add_contract_balance( to.instance(), balance.second, balance.first );
         }
      }
   }
}

void evm::delete_balances( const std::unordered_map< Address, Account >& accounts )
{
   for( std::pair< Address, Account > acc : accounts ) {
      auto id = address_to_id( acc.first );
      if( id.first == 0 )
         continue;

      if(!acc.second.isAlive()){
         get_adapter().delete_contract_balances( id.second );
         get_adapter().contract_suicide( id.second );
         continue;
      }
   }
}

} }
