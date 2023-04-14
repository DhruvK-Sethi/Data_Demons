git:
        @read -p "Enter Comment: " INPUT; \
		git add .
        git commit -m "$$INPUT"
		git push
