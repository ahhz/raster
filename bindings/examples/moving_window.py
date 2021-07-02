
import pronto_python
pronto_python.moving_window_pattern( "input.tif", "output.tif"
                              , 3.0
                              , example.Metric.AreaWeightedPatchSize
                              , example.WindowShape.Square)
pronto_python.moving_window_pattern( "input.tif", "output2.tif"
                              , 3.0
                              , example.Metric.EdgeDensity
                              , example.WindowShape.Square)
