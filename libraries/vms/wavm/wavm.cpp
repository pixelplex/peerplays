#include <wavm.hpp>

namespace vms { namespace wavm {

wavm::wavm( pp_controller::config cfg, vms::wavm::wavm_adapter _adapter ) : adapter(_adapter), contr(cfg, _adapter)
{ }

std::pair<uint64_t, bytes> wavm::exec( const bytes& data)
{ }

std::vector< uint64_t > wavm::get_attracted_contracts( ) const
{ }

void wavm::roll_back_db( const uint32_t& block_number )
{ }

std::map<uint64_t, bytes> wavm::get_contracts( const std::vector<uint64_t>& ids ) const
{ }

bytes wavm::get_code( const uint64_t& id ) const
{ }

} }