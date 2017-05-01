
$(document).ready(function(){
$(".externalPart").click(function () {
    $externalPart = $(this);
    $internalPart = $externalPart.next();
    $internalPart.slideToggle(1000, function () {
        $externalPart.text(function () {
            return $internalPart.is(":visible") ? "-" : "+";
        });
    });
});
});

