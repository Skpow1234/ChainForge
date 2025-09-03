-- ChainForge Database Initialization
-- This script creates the initial database schema

-- Create extensions
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pgcrypto";

-- Create tables
CREATE TABLE IF NOT EXISTS blocks (
    id BIGSERIAL PRIMARY KEY,
    hash BYTEA NOT NULL UNIQUE,
    parent_hash BYTEA,
    number BIGINT NOT NULL,
    timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
    miner_address BYTEA,
    nonce BIGINT,
    difficulty NUMERIC,
    gas_limit BIGINT,
    gas_used BIGINT,
    extra_data BYTEA,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS transactions (
    id BIGSERIAL PRIMARY KEY,
    hash BYTEA NOT NULL UNIQUE,
    block_hash BYTEA REFERENCES blocks(hash),
    block_number BIGINT,
    from_address BYTEA NOT NULL,
    to_address BYTEA,
    value NUMERIC NOT NULL,
    gas_price NUMERIC NOT NULL,
    gas_limit BIGINT NOT NULL,
    gas_used BIGINT,
    nonce BIGINT NOT NULL,
    data BYTEA,
    status SMALLINT DEFAULT 1, -- 1: pending, 2: confirmed, 3: failed
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS addresses (
    id BIGSERIAL PRIMARY KEY,
    address BYTEA NOT NULL UNIQUE,
    balance NUMERIC DEFAULT 0,
    nonce BIGINT DEFAULT 0,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS peers (
    id BIGSERIAL PRIMARY KEY,
    node_id BYTEA NOT NULL UNIQUE,
    address INET NOT NULL,
    port INTEGER NOT NULL,
    last_seen TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    score INTEGER DEFAULT 0,
    banned BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Create indexes
CREATE INDEX IF NOT EXISTS idx_blocks_number ON blocks(number);
CREATE INDEX IF NOT EXISTS idx_blocks_timestamp ON blocks(timestamp);
CREATE INDEX IF NOT EXISTS idx_transactions_block_hash ON transactions(block_hash);
CREATE INDEX IF NOT EXISTS idx_transactions_from_address ON transactions(from_address);
CREATE INDEX IF NOT EXISTS idx_transactions_to_address ON transactions(to_address);
CREATE INDEX IF NOT EXISTS idx_addresses_address ON addresses(address);
CREATE INDEX IF NOT EXISTS idx_peers_address_port ON peers(address, port);

-- Create functions
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ language 'plpgsql';

-- Create triggers
CREATE TRIGGER update_addresses_updated_at 
    BEFORE UPDATE ON addresses 
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- Insert initial data
INSERT INTO addresses (address, balance, nonce) 
VALUES (decode('0000000000000000000000000000000000000000', 'hex'), 0, 0)
ON CONFLICT (address) DO NOTHING;

-- Create views
CREATE OR REPLACE VIEW block_summary AS
SELECT 
    b.number,
    b.hash,
    b.timestamp,
    b.gas_used,
    b.gas_limit,
    COUNT(t.id) as transaction_count,
    SUM(t.value) as total_value
FROM blocks b
LEFT JOIN transactions t ON b.hash = t.block_hash
GROUP BY b.id, b.number, b.hash, b.timestamp, b.gas_used, b.gas_limit
ORDER BY b.number DESC;
