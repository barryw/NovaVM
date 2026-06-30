# NovaVM product site (novavm.whi.dev) static image.
# Build & Release Standard §5 (WAL-32 / WAL-39 / WAL-51): the NovaVM site adopts
# the same containerized deploy as every WHI site (kaniko → GHCR → k8s →
# Cloudflare). The kaniko build context is this `website/` directory.
#
#   docker build -t novavm-website -f Dockerfile .     (run from website/)
#
# The NovaVM site is pre-built static content (no build step — plain HTML/CSS/JS
# plus the WASM browser emulator and PDF/screenshot assets), so this is a single
# nginx stage that copies the site in and serves it. nginx.conf adds the /health
# endpoint the k8s probes require and pins the .wasm mime type for the emulator.
FROM nginx:1.27-alpine
COPY nginx.conf /etc/nginx/conf.d/default.conf
COPY . /usr/share/nginx/html
# The Dockerfile + nginx config are build/deploy inputs, not site content.
RUN rm -f /usr/share/nginx/html/Dockerfile /usr/share/nginx/html/nginx.conf
EXPOSE 80
