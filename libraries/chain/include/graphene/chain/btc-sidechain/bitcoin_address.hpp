#pragma once

#include <graphene/chain/btc-sidechain/types.hpp>
#include <graphene/chain/btc-sidechain/utils.hpp>

using namespace graphene::chain;

 namespace btc_sidechain {

/// Opcodes of numbers in Bitcoin Script
const bytes op_num = {0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f}; // OP_1 - OP_15

/**
 * @class bitcoin_address
 * @brief Describes Bitcoin address
 * 
 */
class bitcoin_address
{

public:

   bitcoin_address() = default;

   /// argument is addres in hex string
   bitcoin_address( const std::string& addr ) : address( addr ), type( determine_type() ),
      raw_address( determine_raw_address() ) {}

   bool operator==( const bitcoin_address& btc_addr ) const;

   bytes get_script() const;

   /// address in hex string
   std::string address;

   /// type of Bitcoin address from enum
   payment_type type;

   /// Address in bytes
   bytes raw_address;

private:

   enum size_segwit_address { P2WSH = 32, P2WPKH = 20 };

   payment_type determine_type();

   bytes determine_raw_address();

   bool check_segwit_address( const size_segwit_address& size ) const;

   bool is_p2pk() const;

   bool is_p2wpkh() const;

   bool is_p2wsh() const;

   bool is_p2pkh() const;

   bool is_p2sh() const;

};

/**
 * @class btc_multisig_address
 * @brief Describes Bitcoin multisig address
 * 
 * https://en.bitcoin.it/wiki/Multisignature
 */
class btc_multisig_address : public bitcoin_address
{

public:

   btc_multisig_address() = default;

   /// arguments are number of required signatures and addres in hex string
   btc_multisig_address( const size_t n_required, const accounts_keys& keys );

   /// amount keys in intersection of given keys and existing address
   size_t count_intersection( const accounts_keys& keys ) const;

   enum address_types { MAINNET_SCRIPT = 5, TESTNET_SCRIPT = 196 };

   enum { OP_0 = 0x00, OP_EQUAL = 0x87, OP_HASH160 = 0xa9, OP_CHECKMULTISIG = 0xae };
   
   bytes redeem_script;

   size_t keys_required = 0;

   /// keys that used in address
   accounts_keys committee_keys;

private:

   void create_redeem_script();

   void create_address();

};

/**
 * @class btc_multisig_segwit_address
 * @brief Describes Bitcoin multisig segwit address (P2WSH)
 * 
 * https://0bin.net/paste/nfnSf0HcBqBUGDto#7zJMRUhGEBkyh-eASQPEwKfNHgQ4D5KrUJRsk8MTPSa
 */
class btc_multisig_segwit_address : public btc_multisig_address
{

public:

   btc_multisig_segwit_address() = default;

   /// arguments are number of required signatures and addres in hex string
   btc_multisig_segwit_address( const size_t n_required, const accounts_keys& keys );

   bool operator==( const btc_multisig_segwit_address& addr ) const;

   std::vector<public_key_type> get_keys();

   bytes committee_script;

private:

   void create_committee_script();

   void create_segwit_address();

   bytes get_address_bytes( const bytes& script_hash );

};

} // namespace btc_sidechain

FC_REFLECT( btc_sidechain::bitcoin_address, (address)(type)(raw_address) );

FC_REFLECT_DERIVED( btc_sidechain::btc_multisig_address, (btc_sidechain::bitcoin_address),
   (redeem_script)(keys_required)(committee_keys) );

FC_REFLECT_DERIVED( btc_sidechain::btc_multisig_segwit_address, (btc_sidechain::btc_multisig_address), (committee_script) );
