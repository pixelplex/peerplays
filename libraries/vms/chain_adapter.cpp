#include <chain_adapter.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/witness_object.hpp>

namespace vms { namespace base {

bool chain_adapter::is_there_account( const uint64_t& account_id ) const
{
   auto& index = db.get_index_type<account_index>().indices().get<by_id>();
   auto itr = index.find( account_id_type( account_id ) );
   return itr != index.end();
}

bool chain_adapter::is_there_contract( const uint64_t& contract_id ) const
{
   auto& index = db.get_index_type<contract_index>().indices().get<by_id>();
   auto itr = index.find( contract_id_type( contract_id ) );
   return itr != index.end();
}

uint32_t chain_adapter::create_contract_obj( const std::set<uint64_t>& allowed_assets )
{
   return db.create<contract_object>( [&]( contract_object& obj ){
      obj.statistics = db.create<contract_statistics_object>([&](contract_statistics_object& s){s.owner = obj.id;}).id;
      obj.allowed_assets = allowed_assets;
   }).get_id().instance.value;
}

void chain_adapter::delete_contract_obj( const uint64_t& contract_id ) {
   auto& index = db.get_index_type<contract_index>().indices().get<by_id>();
   auto itr = index.find( contract_id_type( contract_id ) );
   if ( itr != index.end() )
      db.remove( *itr );
}

uint64_t chain_adapter::get_next_contract_id() const
{
   return db.get_index_type<contract_index>().get_next_id().instance();
}

int64_t chain_adapter::get_account_balance( const uint64_t& account_id, const uint64_t& asset_id ) const
{
   return db.get_balance( account_id_type( account_id ), asset_id_type( asset_id ) ).amount.value;
}

int64_t chain_adapter::get_contract_balance( const uint64_t& contract_id, const uint64_t& asset_id ) const
{
   return db.get_balance( contract_id_type( contract_id ), asset_id_type( asset_id ) ).amount.value;
}

void chain_adapter::change_balance( const contract_or_account_id& from_id, const uint64_t& asset_id, const int64_t& amount )
{
   if( from_id.first )
      db.adjust_balance( contract_id_type( from_id.second ), asset( amount, asset_id_type( asset_id ) ) );
   else
      db.adjust_balance( account_id_type( from_id.second ), asset( amount, asset_id_type( asset_id ) ) );
}

uint64_t chain_adapter::get_id_author_block() const
{
   auto& index = db.get_index_type< witness_index >().indices().get< by_id >();
   auto itr = index.find( db.get_current_block().witness );
   if( index.end() != itr ){
       witness_object witness = *itr;
       return witness.witness_account.instance.value;
   }
   return 0;
}

uint32_t chain_adapter::head_block_num() const
{
   return db.head_block_num();
}

uint32_t chain_adapter::head_block_time() const
{
   return db.head_block_time().sec_since_epoch();
}

bool chain_adapter::evaluating_from_apply_block() const
{
   return db._evaluating_from_block;
}

} }
