extends MeshInstance

onready var tessendorf = load("res://bin/tessendorf.gdns").new()
onready var mdt = MeshDataTool.new()
onready var start_time = OS.get_ticks_msec()

func _ready():
	_copy_mesh()
	mdt.create_from_surface(self.mesh, 0)
	tessendorf.set_mdt(mdt)

func _process(delta):
	var t = OS.get_ticks_msec() - start_time
	tessendorf.update(t / 1000.0)
	self.mesh.surface_remove(0)
	mdt.commit_to_surface(self.mesh)
	
func _copy_mesh():
	#var material = load("res://WaterMaterial.tres")
	var st = SurfaceTool.new()

	st.create_from(self.mesh, 0)
	#st.set_material(material)
	self.mesh = st.commit()