import time
import rasterio

tic = time.clock()

with rasterio.Env():

    # Write an array as a raster band to a new 8-bit file. For
    # the new file's profile, we start with the profile of the source

    with rasterio.open('random_a.tif', 'r') as src_a:
        a = src_a.read()
        profile = src_a.profile

    with rasterio.open('random_b.tif', 'r') as src_b:
        b = src_b.read()

    with rasterio.open('random_c.tif', 'r') as src_c:
        c = src_c.read()

    out = 3 * a + b * c

    profile.update(
       dtype=rasterio.uint8,
       count=1,
       compress='lzw',
       tiled=True,
       blockxsize=256,
       blockysize=256
    )

    with rasterio.open('random_out.tif', 'w', **profile) as dst:
       dst.write(out[0].astype(rasterio.uint8), 1)

toc = time.clock()

print(toc-tic)