// License: MIT (Expat)

"use strict";

function show_bin_file_info(data) {
    if (!data.trim())
        return;

    var obj = $.parseJSON(data);

    var list = ["id", "file_name", "size", "sha256sum"];
    for (var k in list) {
        if (!obj.hasOwnProperty(list[k]))
            return;
    }

    if (!obj.id.trim())
        return;

    // alert(data); // Debug mode!
    $("#" + obj.id).css("cursor", "pointer");
    location.href = $("#" + obj.id).attr("href");
}

function get_bin_file_info() {
    var item = $(this);
    if (item.css("cursor") === "wait")
        return false;

    var data = "id=" + item.attr("id");
    var url = item.attr("href") + "?ajax=file_info";
    item.css("cursor", "wait");
    $.post(url, data, show_bin_file_info);

    var interval = 3000;
    function loop() {
        if (item.css("cursor") === "wait") {
            $.post(url, data, show_bin_file_info);
            setTimeout(loop, interval);
        }
    };
    setTimeout(loop, interval);

    return false;
}

function hashCode(str) {
    var hash = 0;
    if (str.length === 0)
        return hash;

    var char = 0;
    for (var i = 0; i < str.length; ++i) {
        char = str.charCodeAt(i);
        hash = ((hash << 5) - hash) + char;
        hash = hash & hash; // Convert to 32bit integer
    }
    return hash;
}

function update_cells_width() {
    var list = [".finished_at", ".started_at", ".log_files"];
    for (var k in list) {
        var width = 0;
        var hidden = false;
        $.each($("#results_table_container").find(list[k]), function(i, item) {
            width = Math.max(width, $(item).width());
            if (!$(item).text().trim())
                hidden = true;
        });
        $.each($("#results_table_container").find(list[k]), function(i, item) {
            $(item).css("min-width", width + "px");
            if (hidden)
                $(item).hide();
        });
    }
}

function add_item_into_results_table(i, item) {
    var num = +localStorage.results_number;
    localStorage.results_number = num + 1;

    var status = "?";
    if (item.status === "passed")
        status = "✔";
    else if (item.status === "failed")
        status = "✘";
    else if (item.status === "started")
        status = "⚫";

    var hashTag = hashCode(item.target);
    var path = $("#projects_page").find("a").attr("href") + "/" +
                item.project_name + "/" + item.version + "/" + item.target;
    var html =  "<div id=\"" + item.target + "\" class=\"table_item " +
                item.status + "\">\n" +
                    "<li class=\"status\">\n" + status + "</li>\n" +
                    "<li class=\"results\">\n" +
                        "<a id=\"" + hashTag + "\" href=\"" +
                        path + "/" + item.bin_file + "\" class=\"" +
                        item.status + "\">" + item.target + "</a>" +
                    "</li>\n" +
                    "<li class=\"log_files\">\n" +
                        "<a href=\"" + path + "/" + item.log_file + "\">" +
                        item.log_file + "</a>" +
                    "</li>\n" +
                    "<li class=\"started_at\">\n" + item.started + "</li>\n" +
                    "<li class=\"finished_at\">\n" + item.finished + "</li>\n" +
                "</div>\n";
    $("#results_table").append(html);
    $(document).on("click", "#" + hashTag, get_bin_file_info);

    if (item.hasOwnProperty("last_item")) {
        if (item.last_item) {
            $("#show_more").remove();
        }
    }
}

function fill_in_results_table(data) {
    $("#show_more").css("cursor", "pointer");
    var array = $.parseJSON(data);
    $.each(array, add_item_into_results_table);
    update_cells_width();
}

function show_more_results() {
    if ($("#show_more").css("cursor") === "wait")
        return false;

    var len = +localStorage.results_per_page;
    var pos = +localStorage.results_number;
    var data = "pos=" + pos + "&len=" + len;
    $("#show_more").css("cursor", "wait");
    $.post("?ajax=results_list", data, fill_in_results_table);
    return false;
}

function load_results_table() {
    localStorage.results_per_page = 100;
    localStorage.results_number = 0;
    show_more_results();
}

$(document).on("click", "#show_more", show_more_results);
$(document).ready(load_results_table);

