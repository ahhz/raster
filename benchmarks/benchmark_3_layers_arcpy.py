import time
import arcpy
from arcpy import env
from arcpy.sa import *

# Set environment settings
env.workspace = "" # set your workspace
arcpy.env.overwriteOutput = True
# Check out the ArcGIS Spatial Analyst extension license
arcpy.CheckOutExtension("Spatial")

tic = time.clock()

a_file = "random_a.tif"
b_file = "random_b.tif"
c_file = "random_c.tif"
out_file = "output.tif"

a = Raster(a_file)
b = Raster(b_file)
c = Raster(c_file)

out = 3 * a + b * c

out.save(out_file)