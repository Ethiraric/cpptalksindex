var lastqueryinfo = {};

// Load the dropdown lists.
// Calls the API to get the data and creates an entry for each item.
function fillDropdown(items, id, buttonid, element_creator) {
  for (i = 0; i < items.length; ++i) {
    var item = items[i];
    var node = element_creator(id, item, buttonid);
    document.getElementById(`${id}-dropdown-list`).appendChild(node);
  }
}

// Display an error message when clicking on "Search" fails.
function setFilterError(errmsg) {
  if (errmsg == "")
    document.getElementById("searcherror").innerHTML = "";
  else
    document.getElementById("searcherror").innerHTML =
    `<div class="alert alert-danger"><center>${errmsg}</center></div>`;
}

// Load the thumbnail for a link.
// TODO(ethiraric): Find a cleaner way to do that..
function loadThumbnail(link) {
  var imagenode = document.createElement("img");
  var uri = "https://img.youtube.com/vi/" + link.slice(-11) + "/2.jpg";
  imagenode.setAttribute("src", uri);
  var anode = document.createElement("a");
  anode.setAttribute("href", link);
  anode.classList.add("align-self-center");
  anode.appendChild(imagenode);
  return anode;
}

// Get HTML to display the tags from a talk
function getTalkTagHTML(tags) {
  const tagbegin =
    '<span class="rounded-pill border" style="border-width:2px !important;">&nbsp;';
  const tagend = '&nbsp;</span>';
  return tagbegin + tags.join(tagend + '&nbsp;' + tagbegin) + tagend;
}

// Fill the searchresults div.
function showResults(talks) {
  var node = document.getElementById("searchresults");
  node.innerHTML = "";
  if (talks.length == 0) {
    setFilterError("No talk matched your criteria.");
  }
  for (i = 0; i < talks.length; ++i) {
    var talk = talks[i];
    var outerdiv = document.createElement("div");
    outerdiv.classList.add("row", "mb-3", "flex-nowrap");
    outerdiv.appendChild(loadThumbnail(talk.link));
    var descdiv = document.createElement("div");
    descdiv.classList.add("ml-3", "align-self-center");
    descdiv.innerHTML =
      `<b>${talk.speakers.map(x => x.display_name).join(', ')} - ${talk.title}</b></br>
       ${talk.conference} - ${talk.year} </br>
       <a href="${talk.link}">${talk.link} </br></a>
       ${getTalkTagHTML(talk.tags)}`
    outerdiv.appendChild(descdiv);
    node.appendChild(outerdiv);
  }
}

// Build the URL, call it and display results.
function onSearch() {
  var queryinfo = {
    speaker: document.getElementById('speaker-dropdown').getAttribute(
      "speaker-id"),
    conference: document.getElementById('conference-dropdown').innerHTML
      .trim(),
    year: document.getElementById('year-dropdown').innerHTML.trim(),
    tag: document.getElementById('tag-dropdown').innerHTML.trim()
  };

  if (queryinfo.speaker == "Speaker" || queryinfo.speaker == "Speaker (Any)")
    queryinfo.speaker = null;
  if (queryinfo.conference == "Conference" || queryinfo.conference == "Conference (Any)")
    queryinfo.conference = null;
  if (queryinfo.year == "Year" || queryinfo.year == "Year (Any)")
    queryinfo.year = null;
  if (queryinfo.tag == "Tag" || queryinfo.tag == "Tag (Any)")
    queryinfo.tag = null;

  if (JSON.stringify(queryinfo) == JSON.stringify(lastqueryinfo)) {
    return;
  }

  if (queryinfo.speaker == null && queryinfo.conference == null &&
    queryinfo.year == null && queryinfo.tag == null) // no filter
  {
    setFilterError("Please select at least one filter.");
    return;
  }
  if (queryinfo.speaker == null && queryinfo.conference == null&&
    queryinfo.tag == null) // just a "year" filter, too many results
  {
    setFilterError("Please narrow down with more than a year.");
    return;
  }

  setFilterError("");
  var url = '/api/talks?'
  var needsamp = false;

  if (queryinfo.speaker != null) {
    url += 'speaker=' + encodeURIComponent(queryinfo.speaker);
    needsamp = true;
  }
  if (queryinfo.conference != null) {
    url += (needsamp ? '&' : '') + "conference=" + encodeURIComponent(queryinfo.conference);
    needsamp = true;
  }
  if (queryinfo.year != null) {
    url += (needsamp ? '&' : '') + "year=" + encodeURIComponent(queryinfo.year);
    needsamp = true;
  }
  if (queryinfo.tag != null)
    url += (needsamp ? '&' : '') + "tags=" + encodeURIComponent(queryinfo.tag);
  lastqueryinfo = queryinfo;
  jQuery.get(url, function(talks, status) {
    showResults(talks);
  });
}

function onClearFilters() {
    document.getElementById('speaker-dropdown').setAttribute("speaker-id", "Speaker");
    document.getElementById('speaker-dropdown').innerHTML = "Speaker (Any)";
    document.getElementById('conference-dropdown').innerHTML = "Conference (Any)";
    document.getElementById('year-dropdown').innerHTML = "Year (Any)";
    document.getElementById('tag-dropdown').innerHTML = "Tag (Any)";
    document.getElementById("searchresults").innerHTML = "";
    lastqueryinfo = null;
    setFilterError("");
}

function defaultDropdownElementCreator(id, item, buttonid) {
  var node = document.createElement("a");
  node.classList.add("dropdown-item")
  node.id = `${id}-${item}`;
  node.setAttribute("href", "#");
  node.setAttribute("onclick", `onDropdownSelection("${buttonid}", "${item}")`);
  node.innerHTML = item;
  return node;
}

function updateSpeakerDropdown(speaker, id) {
  var dropdown = document.getElementById('speaker-dropdown');
  dropdown.setAttribute("speaker-id", id);
  dropdown.innerHTML = speaker;
}

function speakerDropdownElementCreator(id, speaker, buttonid) {
  var node = document.createElement("a");
  node.classList.add("dropdown-item")
  node.id = `${id}-${speaker.id}`;
  node.setAttribute("href", "#");
  node.setAttribute("onclick",
    `onSpeakerSelection('${speaker.display_name}', '${speaker.id}')`);
  node.innerHTML = speaker.display_name;
  return node;
}

function onSpeakerSelection(speaker, id) {
  updateSpeakerDropdown(speaker, id);
  onSearch();
}

function onDropdownSelection(buttonid, item) {
  document.getElementById(buttonid).innerHTML = item;
  onSearch();
}

window.onload = function() {
  // Load each dropdown
  jQuery.get('/api/filters', function(items, status) {
    fillDropdown(items.speakers, 'speaker', 'speaker-dropdown',
      speakerDropdownElementCreator);
    fillDropdown(items.conferences, 'conference', 'conference-dropdown',
      defaultDropdownElementCreator);
    fillDropdown(items.years, 'year', 'year-dropdown',
      defaultDropdownElementCreator);
    fillDropdown(items.tags, 'tag', 'tag-dropdown',
      defaultDropdownElementCreator);
  })
}

$(document).ready(function() {
  $('#speaker-dropdown-list').children().click(function() {
    onSearch();
  });
});
