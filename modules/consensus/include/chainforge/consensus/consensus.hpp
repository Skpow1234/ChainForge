#pragma once

#include "chainforge/core/block.hpp"
#include "chainforge/core/hash.hpp"
#include <memory>
#include <optional>
#include <functional>
#include <chrono>

namespace chainforge::consensus {

/**
 * Consensus algorithm types
 */
enum class ConsensusType {
    PROOF_OF_WORK,
    PROOF_OF_STAKE,
    DELEGATED_PROOF_OF_STAKE
};

/**
 * Proof of Work difficulty representation
 */
using Difficulty = uint64_t;

/**
 * Mining result
 */
struct MiningResult {
    bool success;
    uint64_t nonce;
    chainforge::core::Hash block_hash;
    std::chrono::milliseconds mining_time;
    uint64_t attempts;
};

/**
 * Mining statistics
 */
struct MiningStats {
    uint64_t total_attempts;
    uint64_t successful_mines;
    std::chrono::milliseconds total_mining_time;
    double average_attempts_per_second;
    uint64_t current_difficulty;
};

/**
 * Difficulty adjustment parameters
 */
struct DifficultyParams {
    uint64_t target_block_time_seconds = 15;     // Target time between blocks
    uint64_t adjustment_interval_blocks = 144;   // Blocks per difficulty adjustment
    uint64_t max_adjustment_factor = 4;          // Maximum difficulty change factor
    uint64_t min_difficulty = 1;                 // Minimum allowed difficulty
    uint64_t max_difficulty = UINT64_MAX;        // Maximum allowed difficulty
};

/**
 * Consensus interface
 */
class Consensus {
public:
    virtual ~Consensus() = default;

    // Configuration
    virtual void set_difficulty(Difficulty difficulty) = 0;
    virtual Difficulty get_difficulty() const = 0;
    virtual void set_difficulty_params(const DifficultyParams& params) = 0;
    virtual const DifficultyParams& get_difficulty_params() const = 0;

    // Mining operations
    virtual MiningResult mine_block(const chainforge::core::Block& block_template) = 0;
    virtual MiningResult mine_block_async(const chainforge::core::Block& block_template,
                                        std::function<void(MiningResult)> callback) = 0;

    // Validation
    virtual bool validate_block(const chainforge::core::Block& block) const = 0;
    virtual bool validate_proof_of_work(const chainforge::core::Hash& block_hash,
                                      uint64_t nonce,
                                      Difficulty difficulty) const = 0;

    // Difficulty adjustment
    virtual Difficulty adjust_difficulty(uint64_t actual_block_time_seconds,
                                       Difficulty current_difficulty) const = 0;
    virtual Difficulty calculate_target_difficulty(const std::vector<uint64_t>& block_times) const = 0;

    // Utility functions
    virtual ConsensusType get_type() const = 0;
    virtual MiningStats get_mining_stats() const = 0;
    virtual void reset_mining_stats() = 0;

    // Mining control
    virtual void stop_mining() = 0;
    virtual bool is_mining_active() const = 0;
};

/**
 * Proof of Work consensus implementation
 */
class ProofOfWork : public Consensus {
public:
    explicit ProofOfWork(Difficulty initial_difficulty = 1);
    ~ProofOfWork() override;

    // Configuration
    void set_difficulty(Difficulty difficulty) override;
    Difficulty get_difficulty() const override;
    void set_difficulty_params(const DifficultyParams& params) override;
    const DifficultyParams& get_difficulty_params() const override;

    // Mining operations
    MiningResult mine_block(const chainforge::core::Block& block_template) override;
    MiningResult mine_block_async(const chainforge::core::Block& block_template,
                                std::function<void(MiningResult)> callback) override;

    // Validation
    bool validate_block(const chainforge::core::Block& block) const override;
    bool validate_proof_of_work(const chainforge::core::Hash& block_hash,
                              uint64_t nonce,
                              Difficulty difficulty) const override;

    // Difficulty adjustment
    Difficulty adjust_difficulty(uint64_t actual_block_time_seconds,
                               Difficulty current_difficulty) const override;
    Difficulty calculate_target_difficulty(const std::vector<uint64_t>& block_times) const override;

    // Utility functions
    ConsensusType get_type() const override { return ConsensusType::PROOF_OF_WORK; }
    MiningStats get_mining_stats() const override;
    void reset_mining_stats() override;

    // Mining control
    void stop_mining() override;
    bool is_mining_active() const override;

private:
    // Internal state
    Difficulty current_difficulty_;
    DifficultyParams difficulty_params_;
    mutable MiningStats mining_stats_;
    bool mining_active_;
    bool stop_requested_;

    // Mining implementation
    MiningResult perform_mining(const chainforge::core::Block& block_template);
    chainforge::core::Hash calculate_block_hash(const chainforge::core::Block& block, uint64_t nonce) const;
    bool meets_difficulty_target(const chainforge::core::Hash& hash, Difficulty difficulty) const;
    Difficulty hash_to_difficulty(const chainforge::core::Hash& hash) const;

    // Thread safety (for async operations)
    mutable std::mutex stats_mutex_;
    mutable std::mutex mining_mutex_;
};

/**
 * Consensus factory functions
 */
std::unique_ptr<Consensus> create_consensus(ConsensusType type, Difficulty initial_difficulty = 1);
std::unique_ptr<ProofOfWork> create_pow_consensus(Difficulty initial_difficulty = 1);

/**
 * Utility functions for PoW
 */
namespace pow {

// Difficulty representations
Difficulty difficulty_from_target(const chainforge::core::Hash& target);
chainforge::core::Hash target_from_difficulty(Difficulty difficulty);

// Mining utilities
uint64_t estimate_mining_time_seconds(Difficulty difficulty, double hash_rate_hps = 1000000.0);
double calculate_network_hash_rate(const std::vector<uint64_t>& block_times,
                                 Difficulty current_difficulty);

// Difficulty adjustment algorithms
Difficulty bitcoin_style_adjustment(uint64_t actual_time_seconds,
                                  uint64_t target_time_seconds,
                                  Difficulty current_difficulty,
                                  uint64_t max_adjustment_factor = 4);

Difficulty ethereum_style_adjustment(const std::vector<uint64_t>& recent_block_times,
                                   uint64_t target_time_seconds,
                                   Difficulty current_difficulty);

} // namespace pow

} // namespace chainforge::consensus
