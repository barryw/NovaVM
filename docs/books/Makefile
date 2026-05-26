BOOKS = basic-user-guide mapping-the-novavm ndk-reference nova-cli-guide nova-fun-n-games

all: pdf

pdf:
	for book in $(BOOKS); do $(MAKE) -C $$book pdf; done

clean:
	for book in $(BOOKS); do $(MAKE) -C $$book clean; done

.PHONY: all pdf clean
