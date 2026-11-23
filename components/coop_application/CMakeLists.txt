idf_component_register(
    SRCS "coop_application.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        coop_config
        bird_counter
        bird_tracker
        bird_movement_processor
)
