#This makefile run the different unit tests

all:
	@printf "\r\n" && \
	echo STARTING TO TEST WITH ALL TESTS && \
	echo ------------------------------- && \
	printf "\r\n"
	
	@$(MAKE) --no-print-directory -C tests_auxiliary_functions/
	@$(MAKE) --no-print-directory -C tests_CAN_BUS_adaptor/
	@$(MAKE) --no-print-directory -C tests_Canard_wrapper/
	@$(MAKE) --no-print-directory -C tests_DroneCAN_service/
	@$(MAKE) --no-print-directory -C tests_DSDL_to_canard_DTO/

clean:
	@$(MAKE) clean --no-print-directory -C tests_auxiliary_functions/
	@$(MAKE) clean --no-print-directory -C tests_CAN_BUS_adaptor/
	@$(MAKE) clean --no-print-directory -C tests_Canard_wrapper/
	@$(MAKE) clean --no-print-directory -C tests_DroneCAN_service/
	@$(MAKE) clean --no-print-directory -C tests_DSDL_to_canard_DTO/