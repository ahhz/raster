
import pronto_python
pronto_python.moving_window_pattern( "input.tif", "output.tif"
                              , 3.0
                              , pronto_python.Metric.AreaWeightedPatchSize
                              , pronto_python.WindowShape.Square)
pronto_python.moving_window_pattern( "input.tif", "output2.tif"
                              , 3.0
                              , pronto_python.Metric.EdgeDensity
                              , pronto_python.WindowShape.Square)
