extends MeshInstance

onready var tessendorf = load("res://bin/tessendorf.gdns").new()
onready var start_time = OS.get_ticks_msec()
onready var shader_mat = self.mesh.surface_get_material(0)

func _ready():	
	tessendorf.create(128)	# ftt resolution
	tessendorf.amplitude = 15.0
	tessendorf.wind_speed = 31.0
	tessendorf.length = 500.0
	tessendorf.lambda = -1.0
	tessendorf.smoothing = 1.0
	tessendorf.wind_direction = Vector2(1.0, 0.0)
	tessendorf.calculate()	# calculate according to variables above this

func _process(delta):
	var t = OS.get_ticks_msec() - start_time
	tessendorf.update(t / 400.0)
	tessendorf.send_displacement(shader_mat, "height_map")
