#include "chainforge/core/transaction.hpp"
#include "chainforge/core/rlp.hpp"
#include "chainforge/crypto/signature.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace chainforge::core {

Transaction::Transaction(const Address& from, const Address& to, const Amount& value)
    : data_{from.data(), to.data(), value.wei(), 21000, 1, {}, 0, {}, 0} {}

Transaction::Transaction(const TransactionData& data) : data_(data) {}

void Transaction::set_from(const Address& from) {
    data_.from = from.data();
    invalidate_cache();
}

void Transaction::set_to(const Address& to) {
    data_.to = to.data();
    invalidate_cache();
}

void Transaction::set_value(const Amount& value) {
    data_.value = value.wei();
    invalidate_cache();
}

void Transaction::set_gas_limit(GasLimit gas_limit) {
    data_.gas_limit = gas_limit;
    invalidate_cache();
}

void Transaction::set_gas_price(GasPrice gas_price) {
    data_.gas_price = gas_price;
    invalidate_cache();
}

void Transaction::set_data(const std::vector<uint8_t>& data) {
    data_.data = data;
    invalidate_cache();
}

void Transaction::set_nonce(uint64_t nonce) {
    data_.nonce = nonce;
    invalidate_cache();
}

void Transaction::set_signature(const std::vector<uint8_t>& signature) {
    data_.signature = signature;
    invalidate_cache();
}

void Transaction::set_chain_id(uint64_t chain_id) {
    data_.chain_id = chain_id;
    invalidate_cache();
}

bool Transaction::sign(const std::vector<uint8_t>& private_key) {
    if (private_key.size() != chainforge::crypto::SECP256K1_PRIVATE_KEY_SIZE) {
        return false;
    }
    chainforge::crypto::Secp256k1PrivateKey sk{};
    std::copy(private_key.begin(), private_key.end(), sk.begin());

    const auto digest = signing_hash().to_bytes();
    auto signed65 = chainforge::crypto::Signature::ecdsa_secp256k1_sign_recoverable(digest, sk);
    if (!signed65.success()) {
        return false;
    }

    std::vector<uint8_t> sig(signed65.value.begin(), signed65.value.end());
    set_signature(sig);

    chainforge::crypto::Secp256k1Signature compact{};
    std::copy(sig.begin(), sig.begin() + 64, compact.begin());
    auto recovered = chainforge::crypto::Signature::ecdsa_secp256k1_recover_public_key(
        digest, compact, sig[64]);
    if (recovered.success()) {
        std::vector<uint8_t> pub(recovered.value.begin(), recovered.value.end());
        set_from(Address::from_public_key(pub));
    }
    return validate_signature();
}

std::vector<uint8_t> Transaction::rlp_encode_unsigned() const {
    std::vector<rlp::Bytes> fields;
    fields.push_back(rlp::encode_uint(data_.nonce));
    fields.push_back(rlp::encode_uint(data_.gas_price));
    fields.push_back(rlp::encode_uint(data_.gas_limit));
    fields.push_back(rlp::encode_address20(data_.to.data()));
    fields.push_back(rlp::encode_uint(data_.value));
    fields.push_back(rlp::encode_bytes(data_.data));
    if (data_.chain_id != 0) {
        // EIP-155 signing payload
        fields.push_back(rlp::encode_uint(data_.chain_id));
        fields.push_back(rlp::encode_uint(0));
        fields.push_back(rlp::encode_uint(0));
    }
    return rlp::encode_list(fields);
}

std::vector<uint8_t> Transaction::rlp_encode_signed() const {
    if (data_.signature.size() != 65) {
        throw std::invalid_argument("Signed RLP requires 65-byte signature");
    }
    const uint8_t v_raw = data_.signature[64];
    uint64_t v = v_raw;
    if (data_.chain_id != 0 && v_raw < 35) {
        // Accept recovery id 0/1 and convert to EIP-155 v
        v = static_cast<uint64_t>(v_raw) + 35 + 2 * data_.chain_id;
    }

    std::vector<uint8_t> r(data_.signature.begin(), data_.signature.begin() + 32);
    std::vector<uint8_t> s(data_.signature.begin() + 32, data_.signature.begin() + 64);

    std::vector<rlp::Bytes> fields;
    fields.push_back(rlp::encode_uint(data_.nonce));
    fields.push_back(rlp::encode_uint(data_.gas_price));
    fields.push_back(rlp::encode_uint(data_.gas_limit));
    fields.push_back(rlp::encode_address20(data_.to.data()));
    fields.push_back(rlp::encode_uint(data_.value));
    fields.push_back(rlp::encode_bytes(data_.data));
    fields.push_back(rlp::encode_uint(v));
    fields.push_back(rlp::encode_bytes(r));
    fields.push_back(rlp::encode_bytes(s));
    return rlp::encode_list(fields);
}

Hash Transaction::signing_hash() const {
    return hash_keccak256(rlp_encode_unsigned());
}

Hash Transaction::calculate_hash() const {
    if (cached_hash_.has_value()) {
        return cached_hash_.value();
    }

    if (data_.signature.size() == 65) {
        cached_hash_ = hash_keccak256(rlp_encode_signed());
    } else {
        cached_hash_ = signing_hash();
    }
    return cached_hash_.value();
}

Transaction Transaction::from_rlp(const std::vector<uint8_t>& rlp_bytes) {
    auto decoded = rlp::decode(rlp_bytes);
    if (!decoded || !decoded->is_list ||
        (decoded->items.size() != 6 && decoded->items.size() != 9)) {
        throw std::invalid_argument("Invalid transaction RLP");
    }

    TransactionData data{};
    data.nonce = rlp::item_as_uint(decoded->items[0]);
    data.gas_price = rlp::item_as_uint(decoded->items[1]);
    data.gas_limit = rlp::item_as_uint(decoded->items[2]);
    {
        auto to = rlp::item_as_bytes(decoded->items[3]);
        if (to.empty()) {
            data.to = {};
        } else if (to.size() == 20) {
            std::copy(to.begin(), to.end(), data.to.begin());
        } else {
            throw std::invalid_argument("Invalid to address in RLP");
        }
    }
    data.value = rlp::item_as_uint(decoded->items[4]);
    data.data = rlp::item_as_bytes(decoded->items[5]);

    if (decoded->items.size() == 9) {
        const uint64_t v = rlp::item_as_uint(decoded->items[6]);
        auto r = rlp::item_as_bytes(decoded->items[7]);
        auto s = rlp::item_as_bytes(decoded->items[8]);
        if (r.size() > 32 || s.size() > 32) {
            throw std::invalid_argument("Invalid r/s length");
        }
        std::vector<uint8_t> sig(65, 0);
        std::copy(r.end() - std::min<size_t>(32, r.size()), r.end(), sig.begin() + (32 - std::min<size_t>(32, r.size())));
        std::copy(s.end() - std::min<size_t>(32, s.size()), s.end(), sig.begin() + 32 + (32 - std::min<size_t>(32, s.size())));

        uint8_t recovery_id = 0;
        if (v >= 35) {
            // EIP-155: v = chainId*2 + 35 + recoveryId
            data.chain_id = (v - 35) / 2;
            recovery_id = static_cast<uint8_t>((v - 35) % 2);
        } else if (v == 27 || v == 28) {
            recovery_id = static_cast<uint8_t>(v - 27);
        } else if (v <= 1) {
            recovery_id = static_cast<uint8_t>(v);
        } else {
            throw std::invalid_argument("Unsupported v value");
        }
        sig[64] = recovery_id;
        data.signature = std::move(sig);
    }

    Transaction tx(data);
    // Recover from address when signature present
    if (tx.validate_signature() && !data.signature.empty()) {
        // from filled inside validate_signature path below if recovery works — set via recover here
        auto msg = tx.signing_hash().to_bytes();
        chainforge::crypto::Secp256k1Signature compact{};
        std::copy(data.signature.begin(), data.signature.begin() + 64, compact.begin());
        auto recovered = chainforge::crypto::Signature::ecdsa_secp256k1_recover_public_key(
            msg, compact, data.signature[64]);
        if (recovered.success()) {
            std::vector<uint8_t> pub(recovered.value.begin(), recovered.value.end());
            auto addr = Address::from_public_key(pub);
            tx.set_from(addr);
        }
    }
    return tx;
}

Amount Transaction::calculate_fee() const {
    return Amount::from_wei(data_.gas_limit * data_.gas_price);
}

bool Transaction::is_contract_creation() const noexcept {
    Address to_addr(data_.to);
    return to_addr.is_zero() && !data_.data.empty();
}

bool Transaction::is_contract_call() const noexcept {
    Address to_addr(data_.to);
    return !to_addr.is_zero() && !data_.data.empty();
}

bool Transaction::is_transfer() const noexcept {
    Address to_addr(data_.to);
    return !to_addr.is_zero() && data_.data.empty();
}

bool Transaction::is_valid() const {
    return validate_addresses() && validate_gas() && validate_amount() && validate_nonce();
}

bool Transaction::validate_signature() const {
    if (data_.signature.empty()) {
        return true; // unsigned allowed for local/dev
    }
    if (data_.signature.size() != 65) {
        return false;
    }

    chainforge::crypto::Secp256k1Signature compact{};
    std::copy(data_.signature.begin(), data_.signature.begin() + 64, compact.begin());
    const uint8_t recovery_id = data_.signature[64];
    auto msg = signing_hash().to_bytes();
    auto recovered = chainforge::crypto::Signature::ecdsa_secp256k1_recover_public_key(
        msg, compact, recovery_id);
    if (!recovered.success()) {
        return false;
    }

    std::vector<uint8_t> pub(recovered.value.begin(), recovered.value.end());
    Address derived = Address::from_public_key(pub);
    // If from is zero, accept recovered; else must match.
    Address from_addr(data_.from);
    if (from_addr.is_zero()) {
        return true;
    }
    return derived == from_addr;
}

bool Transaction::validate_gas() const {
    return data_.gas_limit >= 21000 && data_.gas_price > 0;
}

bool Transaction::validate_amount() const {
    // Zero-value calls/transfers are valid (e.g. contract calls).
    (void)data_.value;
    return true;
}

size_t Transaction::size() const {
    size_t size = sizeof(TransactionData);
    size += data_.data.size();
    return size;
}

bool Transaction::is_too_large() const {
    return size() > MAX_TRANSACTION_SIZE;
}

std::string Transaction::to_string() const {
    std::stringstream ss;
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    Amount value(data_.value);
    ss << "Transaction{"
       << "from: " << from_addr.to_hex()
       << ", to: " << to_addr.to_hex()
       << ", value: " << value.to_string()
       << ", gas_limit: " << data_.gas_limit
       << ", gas_price: " << data_.gas_price
       << ", nonce: " << data_.nonce
       << ", data_size: " << data_.data.size()
       << "}";
    return ss.str();
}

std::string Transaction::to_json() const {
    nlohmann::json j;
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    Amount value(data_.value);
    j["from"] = from_addr.to_hex();
    j["to"] = to_addr.to_hex();
    j["value"] = value.to_string();
    j["gasLimit"] = data_.gas_limit;
    j["gasPrice"] = data_.gas_price;
    j["nonce"] = data_.nonce;
    j["chainId"] = data_.chain_id;
    {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::setfill('0');
        for (uint8_t b : data_.data) {
            oss << std::setw(2) << static_cast<int>(b);
        }
        j["data"] = oss.str();
    }
    if (!data_.signature.empty()) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::setfill('0');
        for (uint8_t b : data_.signature) {
            oss << std::setw(2) << static_cast<int>(b);
        }
        j["signature"] = oss.str();
    }
    j["hash"] = "0x" + calculate_hash().to_hex();

    return j.dump(2);
}

namespace {

std::vector<uint8_t> parse_hex_bytes(std::string hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.erase(0, 2);
    }
    if (hex.size() % 2 != 0) {
        hex.insert(hex.begin(), '0');
    }
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        out.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
    }
    return out;
}

} // namespace

std::optional<Transaction> Transaction::from_json(const std::string& json_str) {
    try {
        const auto j = nlohmann::json::parse(json_str);
        if (!j.contains("from") || !j.contains("to") || !j.contains("value")) {
            return std::nullopt;
        }
        Transaction tx(Address(j.at("from").get<std::string>()),
                       Address(j.at("to").get<std::string>()),
                       Amount::from_string(j.at("value").get<std::string>()));
        if (j.contains("gasLimit")) {
            tx.set_gas_limit(j.at("gasLimit").get<GasLimit>());
        }
        if (j.contains("gasPrice")) {
            tx.set_gas_price(j.at("gasPrice").get<GasPrice>());
        }
        if (j.contains("nonce")) {
            tx.set_nonce(j.at("nonce").get<uint64_t>());
        }
        if (j.contains("chainId")) {
            tx.set_chain_id(j.at("chainId").get<uint64_t>());
        }
        if (j.contains("data") && j.at("data").is_string()) {
            tx.set_data(parse_hex_bytes(j.at("data").get<std::string>()));
        }
        if (j.contains("signature") && j.at("signature").is_string()) {
            tx.set_signature(parse_hex_bytes(j.at("signature").get<std::string>()));
        }
        return tx;
    } catch (...) {
        return std::nullopt;
    }
}

std::string Transaction::to_hex() const {
    // Hex of the hash preimage fields (not RLP yet)
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (uint8_t b : data_.from) oss << std::setw(2) << static_cast<int>(b);
    for (uint8_t b : data_.to) oss << std::setw(2) << static_cast<int>(b);
    auto append_be = [&](uint64_t value) {
        for (int i = 7; i >= 0; --i) {
            oss << std::setw(2) << static_cast<int>((value >> (i * 8)) & 0xFF);
        }
    };
    append_be(data_.value);
    append_be(data_.gas_limit);
    append_be(data_.gas_price);
    append_be(data_.nonce);
    for (uint8_t b : data_.data) oss << std::setw(2) << static_cast<int>(b);
    return oss.str();
}

void Transaction::invalidate_cache() {
    cached_hash_.reset();
}

bool Transaction::validate_addresses() const {
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    return from_addr.is_valid() && (to_addr.is_valid() || is_contract_creation());
}

bool Transaction::validate_nonce() const {
    return true; // For now, always valid
}

// Free functions
std::ostream& operator<<(std::ostream& os, const Transaction& transaction) {
    os << transaction.to_string();
    return os;
}

bool operator==(const Transaction& lhs, const Transaction& rhs) {
    return lhs.calculate_hash() == rhs.calculate_hash();
}

bool operator!=(const Transaction& lhs, const Transaction& rhs) {
    return !(lhs == rhs);
}

Transaction create_transfer_transaction(const Address& from, const Address& to, const Amount& value) {
    return Transaction(from, to, value);
}

Transaction create_contract_transaction(const Address& from, const std::vector<uint8_t>& contract_data) {
    Transaction tx(from, Address::zero(), Amount::zero());
    tx.set_data(contract_data);
    tx.set_gas_limit(53000); // Higher gas limit for contract creation
    return tx;
}

Transaction create_contract_call_transaction(const Address& from, const Address& contract, const std::vector<uint8_t>& call_data) {
    Transaction tx(from, contract, Amount::zero());
    tx.set_data(call_data);
    return tx;
}

bool is_valid_transaction(const Transaction& transaction) {
    return transaction.is_valid();
}

bool is_valid_transaction_data(const TransactionData& data) {
    Transaction tx(data);
    return tx.is_valid();
}

GasLimit estimate_gas(const Transaction& transaction) {
    if (transaction.is_contract_creation()) {
        return 53000 + transaction.payload().size() * 200; // Rough estimate
    } else if (transaction.is_contract_call()) {
        return 21000 + transaction.payload().size() * 68; // Rough estimate
    } else {
        return 21000; // Standard transfer
    }
}

Amount calculate_transaction_fee(const Transaction& transaction) {
    return transaction.calculate_fee();
}

} // namespace chainforge::core
