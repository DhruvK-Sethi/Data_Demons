all: git

git:
	@read -p "Enter commit message: " INPUT; \
	git add .; \
	git commit -m "$$INPUT"; \
	git push;
