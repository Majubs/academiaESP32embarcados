idf_component_register(SRCS "aula-seguranca.c"
                    INCLUDE_DIRS ".")

# Add the credentials file to the binary
target_add_binary_data(${COMPONENT_TARGET} "../credentials/credentials.txt" TEXT)
