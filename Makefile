.PHONY: help build up down dev logs clean restart

# Default target
help: ## Show this help message
	@echo "OverRide Documentation Docker Commands"
	@echo "======================================"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

build: ## Build the production documentation
	@echo "🔨 Building OverRide documentation..."
	docker-compose build

up: ## Start the production documentation server
	@echo "🚀 Starting OverRide documentation server..."
	docker-compose up -d
	@echo "📖 Documentation available at: http://localhost:3000"

down: ## Stop the documentation server
	@echo "🛑 Stopping OverRide documentation server..."
	docker-compose down

dev: ## Start development server with hot reload
	@echo "🔥 Starting OverRide documentation in development mode..."
	docker-compose -f docker-compose.dev.yml up -d
	@echo "🛠️  Development server available at: http://localhost:3000"

dev-down: ## Stop development server
	@echo "🛑 Stopping development server..."
	docker-compose -f docker-compose.dev.yml down

logs: ## Show logs from the documentation container
	docker-compose logs -f override-docs

dev-logs: ## Show logs from the development container
	docker-compose -f docker-compose.dev.yml logs -f override-docs-dev

clean: ## Remove all containers, images and volumes
	@echo "🧹 Cleaning up Docker resources..."
	docker-compose down -v --rmi all
	docker-compose -f docker-compose.dev.yml down -v --rmi all
	docker system prune -f

restart: ## Restart the production server
	@echo "🔄 Restarting OverRide documentation server..."
	docker-compose restart

status: ## Show container status
	@echo "📊 Container Status:"
	docker-compose ps
	@echo ""
	@echo "📊 Development Container Status:"
	docker-compose -f docker-compose.dev.yml ps
