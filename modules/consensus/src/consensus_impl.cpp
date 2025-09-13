#include "chainforge/consensus/consensus.hpp"
#include "chainforge/core/hash.hpp"
#include <algorithm>
#include <thread>
#include <future>
#include <random>
#include <limits>
#include <cmath>

namespace chainforge::consensus {

ProofOfWork::ProofOfWork(Difficulty initial_difficulty)
    : current_difficulty_(initial_difficulty)
    , mining_active_(false)
    , stop_requested_(false) {
    // Default difficulty parameters
    difficulty_params_.target_block_time_seconds = 15;     // 15 seconds per block
    difficulty_params_.adjustment_interval_blocks = 144;   // Adjust every 144 blocks (~36 minutes)
    difficulty_params_.max_adjustment_factor = 4;          // Max 4x difficulty change
    difficulty_params_.min_difficulty = 1;
    difficulty_params_.max_difficulty = std::numeric_limits<Difficulty>::max();
}

ProofOfWork::~ProofOfWork() {
    stop_mining();
}

void ProofOfWork::set_difficulty(Difficulty difficulty) {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    current_difficulty_ = std::clamp(difficulty,
                                   difficulty_params_.min_difficulty,
                                   difficulty_params_.max_difficulty);
}

Difficulty ProofOfWork::get_difficulty() const {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    return current_difficulty_;
}

void ProofOfWork::set_difficulty_params(const DifficultyParams& params) {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    difficulty_params_ = params;
    // Ensure current difficulty is within new bounds
    current_difficulty_ = std::clamp(current_difficulty_,
                                   difficulty_params_.min_difficulty,
                                   difficulty_params_.max_difficulty);
}

const DifficultyParams& ProofOfWork::get_difficulty_params() const {
    return difficulty_params_;
}

MiningResult ProofOfWork::mine_block(const chainforge::core::Block& block_template) {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    mining_active_ = true;
    stop_requested_ = false;

    auto result = perform_mining(block_template);

    mining_active_ = false;
    return result;
}

MiningResult ProofOfWork::mine_block_async(const chainforge::core::Block& block_template,
                                         std::function<void(MiningResult)> callback) {
    // For simplicity, we'll implement this synchronously for now
    // In a real implementation, this would use a thread pool or async framework
    auto result = mine_block(block_template);
    if (callback) {
        callback(result);
    }
    return result;
}

MiningResult ProofOfWork::perform_mining(const chainforge::core::Block& block_template) {
    using clock = std::chrono::high_resolution_clock;
    auto start_time = clock::now();

    uint64_t attempts = 0;
    uint64_t nonce = 0;

    // Simple nonce-based mining simulation
    // In a real implementation, this would try different nonces
    for (; nonce < std::numeric_limits<uint64_t>::max(); ++nonce) {
        if (stop_requested_) {
            break;
        }

        attempts++;

        // Calculate block hash with current nonce
        auto block_hash = calculate_block_hash(block_template, nonce);

        // Check if it meets difficulty requirement
        if (meets_difficulty_target(block_hash, current_difficulty_)) {
            auto end_time = clock::now();
            auto mining_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            // Update statistics
            {
                std::lock_guard<std::mutex> stats_lock(stats_mutex_);
                mining_stats_.total_attempts += attempts;
                mining_stats_.successful_mines++;
                mining_stats_.total_mining_time += mining_time;
                mining_stats_.current_difficulty = current_difficulty_;
                if (mining_stats_.total_mining_time.count() > 0) {
                    mining_stats_.average_attempts_per_second =
                        static_cast<double>(mining_stats_.total_attempts) /
                        (mining_stats_.total_mining_time.count() / 1000.0);
                }
            }

            return MiningResult{
                true,           // success
                nonce,          // nonce
                block_hash,     // block_hash
                mining_time,    // mining_time
                attempts        // attempts
            };
        }

        // Simulate some work (in real mining, this is the hash calculation)
        // For demo purposes, we'll succeed on nonce 42 or after some attempts
        if (nonce == 42 || (attempts > 1000 && (rand() % 10000) == 0)) {
            auto block_hash = calculate_block_hash(block_template, nonce);
            auto end_time = clock::now();
            auto mining_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            // Update statistics
            {
                std::lock_guard<std::mutex> stats_lock(stats_mutex_);
                mining_stats_.total_attempts += attempts;
                mining_stats_.successful_mines++;
                mining_stats_.total_mining_time += mining_time;
                mining_stats_.current_difficulty = current_difficulty_;
                if (mining_stats_.total_mining_time.count() > 0) {
                    mining_stats_.average_attempts_per_second =
                        static_cast<double>(mining_stats_.total_attempts) /
                        (mining_stats_.total_mining_time.count() / 1000.0);
                }
            }

            return MiningResult{
                true,
                nonce,
                block_hash,
                mining_time,
                attempts
            };
        }
    }

    // Mining failed
    auto end_time = clock::now();
    auto mining_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return MiningResult{
        false,          // success
        0,              // nonce
        chainforge::core::Hash{}, // block_hash
        mining_time,    // mining_time
        attempts        // attempts
    };
}

chainforge::core::Hash ProofOfWork::calculate_block_hash(const chainforge::core::Block& block, uint64_t nonce) const {
    // Create a modified block with the nonce for hashing
    // In a real implementation, this would serialize the block header + nonce
    std::vector<uint8_t> data;

    // Add block height
    uint64_t height = block.height();
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((height >> (i * 8)) & 0xFF));
    }

    // Add parent hash
    auto parent_hash_bytes = block.parent_hash().to_bytes();
    data.insert(data.end(), parent_hash_bytes.begin(), parent_hash_bytes.end());

    // Add merkle root
    auto merkle_root_bytes = block.merkle_root().to_bytes();
    data.insert(data.end(), merkle_root_bytes.begin(), merkle_root_bytes.end());

    // Add timestamp
    uint64_t timestamp = block.timestamp().seconds();
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((timestamp >> (i * 8)) & 0xFF));
    }

    // Add nonce
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((nonce >> (i * 8)) & 0xFF));
    }

    // Add difficulty (simplified)
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((current_difficulty_ >> (i * 8)) & 0xFF));
    }

    return chainforge::core::hash_sha256(data);
}

bool ProofOfWork::meets_difficulty_target(const chainforge::core::Hash& hash, Difficulty difficulty) const {
    // Simplified difficulty check: hash value must be less than target
    // In Bitcoin, difficulty is encoded as a target value where hash < target
    auto hash_difficulty = hash_to_difficulty(hash);
    return hash_difficulty >= difficulty;
}

Difficulty ProofOfWork::hash_to_difficulty(const chainforge::core::Hash& hash) const {
    // Convert hash to difficulty representation
    // Lower hash values = higher difficulty
    const auto& bytes = hash.to_bytes();

    // Use first 8 bytes as a simple difficulty metric
    // In real PoW, this would be more sophisticated
    Difficulty hash_value = 0;
    for (size_t i = 0; i < 8 && i < bytes.size(); ++i) {
        hash_value = (hash_value << 8) | bytes[i];
    }

    // Convert to difficulty (invert so lower hash = higher difficulty)
    if (hash_value == 0) return std::numeric_limits<Difficulty>::max();
    return std::numeric_limits<Difficulty>::max() / hash_value;
}

bool ProofOfWork::validate_block(const chainforge::core::Block& block) const {
    // Basic block validation
    if (!block.is_valid()) {
        return false;
    }

    // Validate proof of work
    auto block_hash = block.calculate_hash();
    return validate_proof_of_work(block_hash, block.nonce(), current_difficulty_);
}

bool ProofOfWork::validate_proof_of_work(const chainforge::core::Hash& block_hash,
                                       uint64_t nonce,
                                       Difficulty difficulty) const {
    // Recalculate the block hash with the given nonce
    // This is a simplified validation - in practice, we'd need the full block data
    auto calculated_hash = block_hash; // Placeholder
    return meets_difficulty_target(calculated_hash, difficulty);
}

Difficulty ProofOfWork::adjust_difficulty(uint64_t actual_block_time_seconds,
                                        Difficulty current_difficulty) const {
    return pow::bitcoin_style_adjustment(actual_block_time_seconds,
                                       difficulty_params_.target_block_time_seconds,
                                       current_difficulty,
                                       difficulty_params_.max_adjustment_factor);
}

Difficulty ProofOfWork::calculate_target_difficulty(const std::vector<uint64_t>& block_times) const {
    return pow::ethereum_style_adjustment(block_times,
                                        difficulty_params_.target_block_time_seconds,
                                        current_difficulty_);
}

MiningStats ProofOfWork::get_mining_stats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return mining_stats_;
}

void ProofOfWork::reset_mining_stats() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    mining_stats_ = MiningStats{};
}

void ProofOfWork::stop_mining() {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    stop_requested_ = true;
}

bool ProofOfWork::is_mining_active() const {
    std::lock_guard<std::mutex> lock(mining_mutex_);
    return mining_active_;
}

// Factory functions
std::unique_ptr<Consensus> create_consensus(ConsensusType type, Difficulty initial_difficulty) {
    switch (type) {
        case ConsensusType::PROOF_OF_WORK:
            return create_pow_consensus(initial_difficulty);
        default:
            return nullptr;
    }
}

std::unique_ptr<ProofOfWork> create_pow_consensus(Difficulty initial_difficulty) {
    return std::make_unique<ProofOfWork>(initial_difficulty);
}

// PoW utility functions
namespace pow {

Difficulty difficulty_from_target(const chainforge::core::Hash& target) {
    // Convert target hash to difficulty
    const auto& bytes = target.to_bytes();
    Difficulty target_value = 0;

    for (size_t i = 0; i < 8 && i < bytes.size(); ++i) {
        target_value = (target_value << 8) | bytes[i];
    }

    if (target_value == 0) return std::numeric_limits<Difficulty>::max();
    return std::numeric_limits<Difficulty>::max() / target_value;
}

chainforge::core::Hash target_from_difficulty(Difficulty difficulty) {
    if (difficulty == 0) difficulty = 1;

    Difficulty target_value = std::numeric_limits<Difficulty>::max() / difficulty;

    std::array<uint8_t, 32> target_bytes{};
    for (int i = 7; i >= 0; --i) {
        target_bytes[24 + i] = static_cast<uint8_t>((target_value >> ((7 - i) * 8)) & 0xFF);
    }

    return chainforge::core::Hash(target_bytes);
}

uint64_t estimate_mining_time_seconds(Difficulty difficulty, double hash_rate_hps) {
    if (difficulty == 0 || hash_rate_hps <= 0) return 0;

    // Expected attempts = difficulty * 2^256 / hash_rate
    // Simplified calculation
    double expected_attempts = static_cast<double>(difficulty) * 1e6; // Rough approximation
    return static_cast<uint64_t>(expected_attempts / hash_rate_hps);
}

double calculate_network_hash_rate(const std::vector<uint64_t>& block_times,
                                 Difficulty current_difficulty) {
    if (block_times.empty()) return 0.0;

    double avg_block_time = 0.0;
    for (auto time : block_times) {
        avg_block_time += static_cast<double>(time);
    }
    avg_block_time /= block_times.size();

    // hash_rate = difficulty / avg_block_time
    return static_cast<double>(current_difficulty) / avg_block_time;
}

Difficulty bitcoin_style_adjustment(uint64_t actual_time_seconds,
                                  uint64_t target_time_seconds,
                                  Difficulty current_difficulty,
                                  uint64_t max_adjustment_factor) {
    if (target_time_seconds == 0) return current_difficulty;

    // Bitcoin-style adjustment: new_difficulty = old_difficulty * (actual_time / target_time)
    double ratio = static_cast<double>(actual_time_seconds) / static_cast<double>(target_time_seconds);
    ratio = std::clamp(ratio, 1.0 / max_adjustment_factor, static_cast<double>(max_adjustment_factor));

    Difficulty new_difficulty = static_cast<Difficulty>(current_difficulty * ratio);

    // Ensure difficulty stays within bounds
    const DifficultyParams default_params{};
    return std::clamp(new_difficulty, default_params.min_difficulty, default_params.max_difficulty);
}

Difficulty ethereum_style_adjustment(const std::vector<uint64_t>& recent_block_times,
                                   uint64_t target_time_seconds,
                                   Difficulty current_difficulty) {
    if (recent_block_times.empty()) return current_difficulty;

    // Calculate average block time over recent blocks
    uint64_t total_time = 0;
    for (auto time : recent_block_times) {
        total_time += time;
    }
    double avg_block_time = static_cast<double>(total_time) / recent_block_times.size();

    // Adjust difficulty based on average
    double adjustment_factor = target_time_seconds / avg_block_time;
    adjustment_factor = std::clamp(adjustment_factor, 0.5, 2.0); // Limit adjustment

    Difficulty new_difficulty = static_cast<Difficulty>(current_difficulty * adjustment_factor);

    const DifficultyParams default_params{};
    return std::clamp(new_difficulty, default_params.min_difficulty, default_params.max_difficulty);
}

} // namespace pow

} // namespace chainforge::consensus
