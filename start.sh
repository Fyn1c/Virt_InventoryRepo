#!/bin/bash
set -e 


docker build -t database-container:latest ./database
docker build -t backend-container:latest ./backend
docker build -t frontend-container:latest ./frontend

minikube start --driver=docker

minikube image load database-container:latest
minikube image load backend-container:latest
minikube image load frontend-container:latest

kubectl apply -f deployment.yml

pkill -f "port-forward" || true
kubectl port-forward service/frontend-service 30000:80 > /dev/null 2>&1 &
##kubectl port-forward service/backend-service 30080:8080 > /dev/null 2>&1 &
