var lastqueryinfo = {};

// Load the dropdown lists.
// Calls the API to get the data and creates an entry for each item.
function fillSelect(items, id, element_creator) {
  for (i = 0; i < items.length; ++i) {
    var item = items[i];
    var node = element_creator(item);
    document.getElementById(`${id}-select`).appendChild(node);
  }
}

function setupSelect(item, id, element_creator) {
  fillSelect(item, id, element_creator);
  document.getElementById(`${id}-select`).setAttribute('onchange', 'onSearch()');
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
    var talk_title_html = `<b>${talk.speakers.map(x => x.display_name).join(', ')} - ${talk.title}</b>`;
    var conference_html = `${talk.conference} - ${talk.year}`;
    if (talk.slides)
      conference_html += ` - <a href="${talk.slides}" target="_blank">[slides]</a>`
    var talk_link_html = `<a href="${talk.link}">${talk.link}</a>`;
    descdiv.innerHTML =
      `${talk_title_html}</br>
       ${conference_html}</br>
       ${talk_link_html}</br>
       ${getTalkTagHTML(talk.tags)}`
    outerdiv.appendChild(descdiv);
    node.appendChild(outerdiv);
  }
}

// Build the URL, call it and display results.
function onSearch() {
  var queryinfo = {
    speaker: document.getElementById('speaker-select').value,
    conference: document.getElementById('conference-select').value,
    year: document.getElementById('year-select').value,
    tag: document.getElementById('tag-select').value
  };

  if (queryinfo.speaker == "Speaker")
    queryinfo.speaker = null;
  if (queryinfo.conference == "Conference")
    queryinfo.conference = null;
  if (queryinfo.year == "Year")
    queryinfo.year = null;
  if (queryinfo.tag == "Tag")
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
    document.getElementById('speaker-select').selectedIndex = 0;
    document.getElementById('conference-select').selectedIndex = 0;
    document.getElementById('year-select').selectedIndex = 0;
    document.getElementById('tag-select').selectedIndex = 0;
    $('#searchfilters').find('.selectpicker').selectpicker('refresh');

    document.getElementById("searchresults").innerHTML = "";
    lastqueryinfo = null;
    setFilterError("");
}

function speakerDropdownElementCreator(speaker) {
  var node = document.createElement("option");

  node.innerHTML = speaker.display_name;
  node.value = speaker.id;
  node.setAttribute("class", "cspeaker");

  return node;
}

function defaultOptionElementCreator(item) {
  var node = document.createElement("option");

  node.innerHTML = item;
  node.value = item;
  node.setAttribute("class", "cspeaker");
  return node;
}

window.onload = function() {
  // Load each dropdown
  jQuery.get('/api/filters', function(items, status) {
    setupSelect(items.speakers, 'speaker', speakerDropdownElementCreator);
    setupSelect(items.conferences, 'conference', defaultOptionElementCreator);
    setupSelect(items.years, 'year', defaultOptionElementCreator);
    setupSelect(items.tags, 'tag', defaultOptionElementCreator);
    $('#searchfilters').find('.selectpicker').selectpicker('refresh');
  })
}
