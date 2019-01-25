extends MeshInstance

onready var tessendorf = load("res://bin/tessendorf.gdns").new()
onready var mdt = MeshDataTool.new()
onready var start_time = OS.get_ticks_msec()

var mat

func _ready():
	_init_mesh()
	mdt.create_from_surface(self.mesh, 0)
	mat = self.mesh.surface_get_material(0)

func _process(delta):
	var t = OS.get_ticks_msec() - start_time
	tessendorf.update(t / 1000.0, mdt, mat)
	self.mesh.surface_remove(0)
	mdt.commit_to_surface(self.mesh)
	
"""
It's not possible to alter PrimitiveMesh and I'm
too lazy to code mesh creating function, so I transform it
to ArrayMesh using SurfaceTool
"""
func _init_mesh():
	var st = SurfaceTool.new()
	st.create_from(self.mesh, 0)
	self.mesh = st.commit()