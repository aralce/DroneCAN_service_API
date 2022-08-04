#This makefile run the different unit tests

all:
	@$(MAKE) --no-print-directory -C test_publish_message_general/
	@$(MAKE) --no-print-directory -C tests_publish_node_status/
	@$(MAKE) --no-print-directory -C tests_publish_paramGetSet/

clean:
	@$(MAKE) clean --no-print-directory -C test_publish_message_general/
	@$(MAKE) clean --no-print-directory -C tests_publish_node_status/
	@$(MAKE) clean --no-print-directory -C tests_publish_paramGetSet/