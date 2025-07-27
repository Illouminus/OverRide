# OverRide Documentation Docker Setup

This directory contains Docker configuration for running the OverRide documentation website.

## Quick Start

### Production Mode
```bash
# Start the documentation server
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the server
docker-compose down
```

### Development Mode
```bash
# Start development server with hot reload
docker-compose -f docker-compose.dev.yml up -d

# Stop development server
docker-compose -f docker-compose.dev.yml down
```

### Using Makefile (Recommended)
```bash
# See all available commands
make help

# Start production server
make up

# Start development server  
make dev

# View logs
make logs

# Stop servers
make down
```

## Services

### Production (`override-docs`)
- **Port**: 3000
- **Image**: Multi-stage build with Nginx
- **Purpose**: Optimized for production deployment
- **Features**: Gzip compression, security headers, caching

### Development (`override-docs-dev`)  
- **Port**: 3000
- **Image**: Node.js with hot reload
- **Purpose**: Development with live changes
- **Features**: Volume mounting, file watching

## URLs

- **Documentation**: http://localhost:3000
- **Health Check**: http://localhost:3000/health (if implemented)

## Docker Commands Reference

```bash
# Build images
docker-compose build

# Start services in background
docker-compose up -d

# View logs
docker-compose logs -f override-docs

# Stop services
docker-compose down

# Remove everything (containers, images, volumes)
docker-compose down -v --rmi all

# Restart services
docker-compose restart
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `NODE_ENV` | `production` | Node.js environment |
| `CHOKIDAR_USEPOLLING` | `true` | Enable file polling for hot reload |

## Troubleshooting

### Port 3000 already in use
```bash
# Find process using port 3000
lsof -i :3000

# Kill the process
kill -9 <PID>

# Or use different port
docker-compose up -d --env-file .env.local
```

### Container won't start
```bash
# Check logs
docker-compose logs override-docs

# Rebuild from scratch
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```

### Hot reload not working in development
Make sure you're using the development compose file:
```bash
docker-compose -f docker-compose.dev.yml up -d
```

## Performance

### Production Optimizations
- Multi-stage Docker build
- Nginx serving static files
- Gzip compression enabled
- Asset caching headers
- Minimal image size

### Development Features  
- Live file watching
- Hot module replacement
- Volume mounting for instant changes
- Development dependencies included
