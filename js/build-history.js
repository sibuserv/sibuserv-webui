// License: MIT (Expat)

"use strict";

function update_cells_width() {
    var list = [".finished_at", ".started_at"];
    for (var k in list) {
        var width = 0;
        var hidden = false;
        $.each($("#builds_table_container").find(list[k]), function(i, item) {
            width = Math.max(width, $(item).width());
            if (!$(item).text().trim())
                hidden = true;
        });
        $.each($("#builds_table_container").find(list[k]), function(i, item) {
            $(item).css("min-width", width + "px");
            if (hidden)
                $(item).hide();
        });
    }
}

function add_item_into_builds_table(i, item) {
    var num = +localStorage.builds_number;
    localStorage.builds_number = num + 1;

    var status = "?";
    if (item.status === "passed")
        status = "✔";
    else if (item.status === "failed")
        status = "✘";
    else if (item.status === "started")
        status = "⚫";

    var html =  "<a id=\"" + item.project_name + "\" class=\"table_item " +
                item.status + "\" href=\"" +
                $("#projects_page").find("a").attr("href") + "/" +
                item.project_name + "/" + item.version + "\">\n" +
                    "<li class=\"status\">\n" + status + "</li>\n" +
                    "<li class=\"version\">\n" + item.version + "</li>\n" +
                    "<li class=\"started_at\">\n" + item.started + "</li>\n" +
                    "<li class=\"finished_at\">\n" + item.finished + "</li>\n" +
                "</a>\n";
    $("#builds_table").append(html);

    if (item.hasOwnProperty("last_item")) {
        if (item.last_item) {
            $("#show_more").remove();
        }
    }
}

function fill_in_builds_table(data) {
    $("#show_more").css("cursor", "pointer");
    var array = $.parseJSON(data);
    $.each(array, add_item_into_builds_table);
    update_cells_width();
}

function show_more_builds() {
    if ($("#show_more").css("cursor") === "wait")
        return false;

    var len = +localStorage.builds_per_page;
    var pos = +localStorage.builds_number;
    var data = "pos=" + pos + "&len=" + len;
    $("#show_more").css("cursor", "wait");
    $.post("?ajax=builds_list", data, fill_in_builds_table);
    return false;
}

function load_builds_table() {
    localStorage.builds_per_page = 20;
    localStorage.builds_number = 0;
    show_more_builds();
}

$(document).on("click", "#show_more", show_more_builds);
$(document).ready(load_builds_table);

