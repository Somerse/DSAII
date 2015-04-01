Nathanial Schwab
----------------



John Watson
----------------
Implemented collisions causing events. Collided models and their bounding boxes
are now moved to the botom center of the screen, and any model that was collided
and has rotation stops rotating. Each model and its bounding box have their own
event based on collision thanks to a switch statement.