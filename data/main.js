/*
$("ul").on("click", ".init", function() {
    $(this).closest("ul").children('li:not(.init)').toggle();
});

var allOptions = $("ul").children('li:not(.init)');
$("ul").on("click", "li:not(.init)", function() {
    allOptions.removeClass('selected');
    $(this).addClass('selected');
    $("ul").children('.init').html($(this).html());
    allOptions.toggle();
});
*/


window.addEventListener('load', onLoad);

function onLoad(event) {
    //document.querySelector('.init').addEventListener('click', showList);
    for(item of document.querySelectorAll('.combo-box ul li')) {
        console.log(item)
        item.addEventListener('click', selectItem)
    }
}

// Close combo-box if click is outside
document.addEventListener('click', () => { document.querySelector('.combo-box').classList.remove('active')})

document.querySelector('.combo-box').addEventListener('click', showList)

function showList(event) {
    console.log(event.target)
    event.target.classList.toggle('active');
    event.stopPropagation();
}

function selectItem(event) {
    console.log(event.target)
    document.querySelector('.selected').innerHTML = event.target.innerHTML;
    document.querySelector('.combo-box').classList.remove('active');
    event.stopPropagation();
}

/*
function showList() {
    console.log('ghesboro')

    for(item of document.querySelectorAll('ul > li:not(.init)')) {
        console.log(item.innerText)
        item.style.display = 'inline-block';
    }
}
*/