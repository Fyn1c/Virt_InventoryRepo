FROM node:22-alpine

WORKDIR /app


RUN npm create vite@latest . -- --template react-ts && npm install

EXPOSE 5173

CMD ["npx", "vite", "--host"]