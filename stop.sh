#!/bin/bash

pkill -f "port-forward" || true

minikube stop

