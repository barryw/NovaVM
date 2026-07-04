all: pdf

pdf:
	dotnet run --project ../../../e6502.Nova -c Release -- docs basic-user-guide --repo ../../..

clean:
	rm -f assembled.md novabasic_user_guide.pdf

.PHONY: all pdf clean
