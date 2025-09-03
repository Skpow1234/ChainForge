# Grafana Configuration for ChainForge

This directory contains the Grafana configuration for monitoring ChainForge services.

## Structure

```bash
deploy/grafana/
├── provisioning/
│   ├── datasources/
│   │   └── datasources.yml      # Data source configurations
│   └── dashboards/
│       └── dashboards.yml       # Dashboard provisioning config
├── dashboards/
│   └── chainforge/
│       └── chainforge-overview.json  # Main ChainForge dashboard
└── README.md                    # This file
```

## Data Sources

- **Prometheus**: Metrics collection from ChainForge services
- **Jaeger**: Distributed tracing data

## Dashboards

- **ChainForge Overview**: Main dashboard showing blockchain metrics
  - Block height
  - Transaction rate
  - Active P2P peers

## Configuration

The configuration is automatically loaded when Grafana starts via Docker Compose.
Dashboards are placed in the `chainforge` folder for organization.

## Access

- **URL**: <http://localhost:3000>
- **Username**: admin
- **Password**: chainforge_grafana_pass (set in docker-compose.yml)
