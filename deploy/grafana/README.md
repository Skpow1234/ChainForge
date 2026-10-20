# Grafana Configuration for Tokiwa

This directory contains the Grafana configuration for monitoring Tokiwa services.

## Structure

```bash
deploy/grafana/
├── provisioning/
│   ├── datasources/
│   │   └── datasources.yml      # Data source configurations
│   └── dashboards/
│       └── dashboards.yml       # Dashboard provisioning config
├── dashboards/
│   └── tokiwa/
│       └── tokiwa-overview.json  # Main Tokiwa dashboard
└── README.md                    # This file
```

## Data Sources

- **Prometheus**: Metrics collection from Tokiwa services
- **Jaeger**: Distributed tracing data

## Dashboards

- **Tokiwa Overview**: Main dashboard showing blockchain metrics
  - Block height
  - Transaction rate
  - Active P2P peers

## Configuration

The configuration is automatically loaded when Grafana starts via Docker Compose.
Dashboards are placed in the `tokiwa` folder for organization.

## Access

- **URL**: <http://localhost:3000>
- **Username**: admin
- **Password**: tokiwa_grafana_pass (set in docker-compose.yml)
