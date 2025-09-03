# Prometheus Configuration for ChainForge

This directory contains the Prometheus configuration for monitoring ChainForge services.

## Files

- `prometheus.yml` - Main Prometheus configuration
- `README.md` - This documentation

## Configuration

The `prometheus.yml` file configures:

- **Global settings**: 15s scrape interval
- **Targets**:
  - Prometheus itself (localhost:9090)
  - ChainForge Node (chainforge-node:8080)
  - ChainForge Explorer (chainforge-explorer:8081)
  - PostgreSQL (postgres:5432)
  - Redis (redis:6379)

## Metrics Endpoints

- **ChainForge Node**: `/metrics` on port 8080
- **ChainForge Explorer**: `/metrics` on port 8081

## Access

- **URL**: <http://localhost:9090>
- **Targets**: <http://localhost:9090/targets>
- **Graph**: <http://localhost:9090/graph>

## Data Retention

- **Retention**: 200 hours (8.33 days)
- **Storage**: `/prometheus` volume in container
