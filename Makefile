test:
	@cp build/dpll tests
	@cd tests && python3 test.py
