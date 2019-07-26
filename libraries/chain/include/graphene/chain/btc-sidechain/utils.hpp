#pragma once

#include <graphene/chain/btc-sidechain/types.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/crypto/elliptic.hpp>

namespace btc_sidechain {

/// parse hex string to bytes
bytes parse_hex( const std::string& str );

std::vector<bytes> get_pubkey_from_redeemScript( bytes script );

bytes public_key_data_to_bytes( const fc::ecc::public_key_data& key );

} // namespace btc_sidechain
