idf_component_register(
    SRCS "bird_movement_processor.cpp"
    INCLUDE_DIRS "include"
    REQUIRES bird_tracker direction_detector bird_counter
)