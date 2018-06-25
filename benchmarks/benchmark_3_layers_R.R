if(!require("raster")) install.packages("raster")
if(!require("rgdal")) install.packages("rgdal")

library(raster)

tic <- proc.time()

a_file <- "random_a.tif"
b_file <- "random_b.tif"
c_file <- "random_c.tif"

out_file <- "output.tif"

a <- raster(a_file, band=1)
b <- raster(b_file, band=1)
c <- raster(c_file, band=1)

out <- 3 * a + b * c

writeRaster(out, out_file, "GTiff", overwrite=TRUE)

toc <- proc.time()

print(toc - tic)