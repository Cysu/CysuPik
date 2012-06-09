$(document).ready(function() {
    var dirEntries = [
        ['Global', 'Point', 'Algebraic', 'Geometrical', 'Morphological', 'Neighbor'],
        [
            ['Undo/Redo', 'Compare'],
            ['Anticolor', 'Threshold', 'Show Histogram', 'Histogram Equalization'],
            ['Addition', 'Subtraction', 'Translation'],
            ['Horizontal Mirror', 'Vertical Mirror', 'Scaling', 'Rotation', 'Perspective'],
            ['Erosion', 'Dilation', 'Open', 'Close', 'Thining'],
            ['Roberts', 'Sobel', 'Canny', 'Averaging Filter', 'Gaussian Filter', 'Median Filter']
        ],
        [
            [],
            ['anticolor.png', 'threshold.png', 'show_histogram.png', 'histogram_equalization.png'],
            ['addition.png', 'subtraction.png', 'translation.png'],
            ['horizontal_mirror.png', 'vertical_mirror.png', 'scaling.png', 'rotation.png', 'perspective.png'],
            ['erosion.png', 'dilation.png', 'open_operation.png', 'close_operation.png', 'thinning.png'],
            ['roberts.png', 'sobel.png', 'canny.png', 'neighborhood_averaging.png', 'neighborhood_gaussian.png', 'neighborhood_median.png']
        ]
    ];

    // Create the level-1 directory.
    for (var i = 0; i < dirEntries[0].length; i += 1) {
        $("#dir-lv1").append(
            '<div class="dir-entry dir-entry-lv1" id="dir-entry-lv1-'+i+'">'+
                '<h1>'+dirEntries[0][i]+'</h1>'+
            '</div>'
        );
    }

    var lv1num = -1, lv2num = -1;

    // Create the level-2 directory when click level-1 directory entries.
    $(".dir-entry-lv1").click(function() {
        if (lv1num !== -1) {
            $("#dir-entry-lv1-"+lv1num).removeClass("current");
        }
        $(this).addClass("current");

        lv1num = parseInt($(this).attr("id").substring(14));
        $("#dir-lv2").hide();
        $("#dir-lv2").empty();
        for (var i = 0; i < dirEntries[1][lv1num].length; i += 1) {
            $("#dir-lv2").append(
                '<div class="dir-entry dir-entry-lv2" id="dir-entry-lv2-'+i+'">'+
                    '<h2>'+dirEntries[1][lv1num][i]+'</h2>'+
                '</div>'
            );
        }
        $(".dir-entry-lv2").hover(
            function() {
                lv2num = parseInt($(this).attr("id").substring(14));
                $("#content").hide();
                if (lv1num !== 0) {
                    // Not global.
                    $("#content").append(
                        '<img class="preview-image" src="images/'+dirEntries[2][lv1num][lv2num]+'">'
                    );
                } else {
                    // Global.
                    if (lv2num === 0) {
                        // Undo/Redo
                        $("#content").append(
                            '<h3>Undo/Redo</h3>'+
                            '<p class="preview-passage">'+
                                'Use <strong>Ctrl+Z</strong> and <strong>Ctrl+Y</strong> to perform undo and redo actions.'+
                            '</p>'
                        );
                    } else if (lv2num === 1) {
                        // Compare
                        $("#content").append(
                            '<h3>Compare</h3>'+
                            '<p class="preview-passage">'+
                                'Click on the <strong>left button</strong> to compare with the original image.'+
                            '</p>'
                        );
                    }
                }
                $("#content").fadeIn('fast');
            },
            function() {
                $("#content").empty();
            }
        );

        $("#dir-lv2").fadeIn('normal');
    });

});
