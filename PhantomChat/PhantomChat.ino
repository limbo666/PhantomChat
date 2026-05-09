#include <WiFi.h>

#include <DNSServer.h>

#include <WebServer.h>

#include <Adafruit_NeoPixel.h>



// Updated to Pin 10 for your specific ESP32-C3 Zero board

#define LED_PIN 10

#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);



const char* ssid = "PhantomChat";

const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 4, 1);



DNSServer dnsServer;

WebServer server(80);



String chatHistory = "";

String knownUIDs = "";



void blinkLED(int r, int g, int b) {

  pixels.setPixelColor(0, pixels.Color(r, g, b));

  pixels.show();

  delay(150);

  pixels.setPixelColor(0, pixels.Color(0, 0, 0));

  pixels.show();

}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0'>
<style>
  html, body { height: 100%; margin: 0; padding: 0; }
  body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background: #0d0d0d; color: #e0e0e0; display: flex; flex-direction: column; overflow: hidden; }
  
  .header { display: flex; padding: 12px 16px; background: #1a1a1a; align-items: center; justify-content: space-between; border-bottom: 1px solid #2a2a2a; z-index: 10; box-shadow: 0 4px 10px rgba(0,0,0,0.3); }
  h2 { margin: 0; font-size: 1.2rem; font-weight: 600; color: #ffffff; letter-spacing: 0.5px; }
  .nick-input { padding: 8px 12px; border-radius: 12px; border: 1px solid #333; background: #262626; color: #fff; width: 100px; text-align: center; outline: none; font-size: 0.9rem; transition: border 0.2s; }
  .nick-input:focus { border-color: #005cbf; }
  
  #chat { flex: 1; overflow-y: auto; padding: 16px; display: flex; flex-direction: column; gap: 14px; background: #121212; }
  .msg-row { display: flex; width: 100%; }
  .bubble { max-width: 75%; padding: 10px 14px; border-radius: 18px; word-wrap: break-word; box-shadow: 0 2px 5px rgba(0,0,0,0.2); font-size: 0.95rem; line-height: 1.4; position: relative; cursor: pointer;}
  .received { justify-content: flex-start; }
  .sent { justify-content: flex-end; }
  .sent .bubble { background: #0066cc; color: #fff; border-bottom-right-radius: 4px; }
  .nick { font-size: 0.75rem; opacity: 0.75; margin-bottom: 4px; font-weight: 600; color: #fff; letter-spacing: 0.2px; }
  .private-tag { color: #ff66ff; font-weight: bold; font-size: 0.7rem; text-transform: uppercase; letter-spacing: 0.5px; }
  
  /* Reaction Styling */
  .rxns { display: flex; flex-wrap: wrap; gap: 4px; margin-top: 6px; }
  .rxns span { background: rgba(0,0,0,0.25); border-radius: 12px; padding: 2px 6px; font-size: 0.8rem; border: 1px solid rgba(255,255,255,0.1); }
  #rxn-menu { display: none; position: fixed; background: #2a2a2a; padding: 10px 16px; border-radius: 25px; box-shadow: 0 8px 20px rgba(0,0,0,0.6); gap: 16px; z-index: 100; border: 1px solid #444; }
  #rxn-menu span { font-size: 1.6rem; cursor: pointer; transition: transform 0.1s; user-select: none;}
  #rxn-menu span:active { transform: scale(1.3); }
  
  .footer { display: flex; flex-direction: column; background: #1a1a1a; border-top: 1px solid #2a2a2a; padding: 10px 16px 20px 16px; gap: 10px; z-index: 10; }
  .disclaimer { font-size: 0.7rem; color: #666; text-align: center; padding-bottom: 4px; }
  .controls-row { display: flex; justify-content: space-between; align-items: center; }
  .btn-pill { padding: 6px 12px; border-radius: 15px; background: #262626; color: #aaa; border: 1px solid #333; cursor: pointer; font-size: 0.8rem; font-weight: 600; outline: none; transition: all 0.2s; }
  .btn-pill.active { background: #004d40; color: #00e676; border-color: #00e676; }
  .target-wrapper { display: flex; align-items: center; gap: 8px; background: #262626; border-radius: 15px; padding: 4px 12px; border: 1px solid #333; }
  .target-wrapper span { font-size: 0.8rem; color: #aaa; font-weight: 600; }
  select#targetUser { background: transparent; color: #fff; border: none; outline: none; font-size: 0.85rem; font-weight: 600; max-width: 110px; cursor: pointer; appearance: none; }
  
  form { display: flex; gap: 10px; align-items: flex-end; }
  input[type=text]#msg { flex: 1; padding: 14px 18px; border-radius: 24px; border: 1px solid #333; background: #262626; color: #fff; outline: none; font-size: 1rem; box-shadow: inset 0 2px 4px rgba(0,0,0,0.1); transition: border 0.2s; }
  input[type=text]#msg:focus { border-color: #0066cc; }
  button[type=submit] { width: 48px; height: 48px; border-radius: 50%; background: #0066cc; color: #fff; border: none; display: flex; justify-content: center; align-items: center; cursor: pointer; flex-shrink: 0; box-shadow: 0 4px 8px rgba(0,102,204,0.3); transition: transform 0.1s, background 0.2s; }
  button[type=submit]:active { transform: scale(0.95); background: #0052a3; }
</style>
</head>
<body>
  <div id='rxn-menu'>
    <span onclick='sendRxn("👍")'>👍</span>
    <span onclick='sendRxn("❤️")'>❤️</span>
    <span onclick='sendRxn("😂")'>😂</span>
    <span onclick='sendRxn("😮")'>😮</span>
    <span onclick='sendRxn("👎")'>👎</span>
  </div>

  <div class='header'>
    <h2>PhantomChat</h2>
    <input type='text' id='nick' class='nick-input' placeholder='Nickname' maxlength='15'>
  </div>
  
  <div id='chat'></div>
  
  <div class='footer'>
    <div class='disclaimer'>Offline, anonymous network. No data retained.</div>
    <div class='controls-row'>
      <button id='sndToggle' class='btn-pill'>Sound: OFF</button>
      <div class='target-wrapper'>
        <span>To:</span>
        <select id='targetUser'><option value='all'>Everyone</option></select>
      </div>
    </div>
    <form id='form'>
      <input type='text' id='msg' placeholder='Type a message...' autocomplete='off' required>
      <button type='submit' title='Send'>
        <svg viewBox='0 0 24 24' width='20' height='20' fill='white'><path d='M2.01 21L23 12 2.01 3 2 10l15 2-15 2z'/></svg>
      </button>
    </form>
  </div>

  <script>
    function safeGet(k, def) { try { return localStorage.getItem(k) || def; } catch(e) { return def; } }
    function safeSet(k, v) { try { localStorage.setItem(k, v); } catch(e) {} }

    let uid = safeGet('uid', Math.random().toString(36).substring(2, 9));
    safeSet('uid', uid);
    
    let defaultNick = 'Anon-' + uid.substring(0, 4).toUpperCase();
    let nickField = document.getElementById('nick');
    nickField.value = safeGet('nick', defaultNick);
    nickField.addEventListener('input', () => safeSet('nick', nickField.value));

    function getHslaColor(str) {
      let hash = 0;
      for (let i = 0; i < str.length; i++) hash = str.charCodeAt(i) + ((hash << 5) - hash);
      return `hsl(${Math.abs(hash) % 360}, 65%, 35%)`; 
    }

    let sndEnabled = safeGet('snd', 'false') === 'true';
    let sndBtn = document.getElementById('sndToggle');
    let audioCtx;
    let knownMsgCount = 0;
    let lastKnownUsers = "";

    function updateSndUI() {
      sndBtn.innerHTML = sndEnabled ? 'Sound: ON' : 'Sound: OFF';
      if(sndEnabled) sndBtn.classList.add('active'); else sndBtn.classList.remove('active');
    }
    updateSndUI();

    function playBeep() {
      if (!sndEnabled) return;
      try {
        if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        if (audioCtx.state === 'suspended') audioCtx.resume();
        let osc = audioCtx.createOscillator();
        let gain = audioCtx.createGain();
        osc.connect(gain); gain.connect(audioCtx.destination);
        osc.type = 'sine'; osc.frequency.setValueAtTime(880, audioCtx.currentTime); 
        gain.gain.setValueAtTime(0.2, audioCtx.currentTime);
        osc.start(); gain.gain.exponentialRampToValueAtTime(0.00001, audioCtx.currentTime + 0.1);
        osc.stop(audioCtx.currentTime + 0.1);
      } catch(e) {} 
      if(navigator.vibrate) try { navigator.vibrate([100]); } catch(e) {}
    }

    sndBtn.onclick = () => { sndEnabled = !sndEnabled; safeSet('snd', sndEnabled); updateSndUI(); if (sndEnabled) playBeep(); };

    fetch('/join', {method: 'POST', body: new URLSearchParams({uid: uid})}).catch(()=>{});

    // Reaction Menu Logic
    let rxnMenu = document.getElementById('rxn-menu');
    let activeMsgId = null;

    document.getElementById('chat').addEventListener('click', (e) => {
      let bubble = e.target.closest('.bubble');
      if (bubble) {
        activeMsgId = bubble.parentElement.getAttribute('data-msgid');
        rxnMenu.style.display = 'flex';
        // Position menu above the tap, keep it inside screen bounds
        let x = e.clientX - 100; 
        let y = e.clientY - 60;
        if(x < 10) x = 10;
        if(x > window.innerWidth - 220) x = window.innerWidth - 220;
        rxnMenu.style.left = x + 'px';
        rxnMenu.style.top = y + 'px';
        e.stopPropagation();
      }
    });

    document.addEventListener('click', () => { rxnMenu.style.display = 'none'; });

    window.sendRxn = (emoji) => {
      if(activeMsgId) {
        fetch('/react', {method: 'POST', body: new URLSearchParams({id: activeMsgId, e: emoji})}).then(update);
      }
    };

    function update() {
      fetch('/msgs').then(r=>r.text()).then(html => {
        let c = document.getElementById('chat');
        let isAtBottom = (c.scrollHeight - c.scrollTop <= c.clientHeight + 20);
        
        let temp = document.createElement('div');
        temp.innerHTML = html;
        let rows = temp.querySelectorAll('.msg-row');
        let currentUsers = {};
        let filteredRowsCount = 0;
        
        rows.forEach(row => {
          let rowUid = row.getAttribute('data-uid');
          let targetUid = row.getAttribute('data-target') || 'all';
          let bubble = row.querySelector('.bubble');
          let nickEl = row.querySelector('.nick');
          
          if (rowUid !== uid) {
             let rawNick = nickEl.innerText.replace(' [PRIVATE]', '').trim();
             currentUsers[rowUid] = rawNick;
          }

          if (targetUid !== 'all' && targetUid !== uid && rowUid !== uid) { row.remove(); return; }
          filteredRowsCount++;

          if (targetUid !== 'all') bubble.style.border = '2px solid #ff66ff'; 

          if (rowUid === uid) {
            row.className = 'msg-row sent';
            bubble.style.backgroundColor = '#0066cc'; 
            bubble.style.borderBottomLeftRadius = '18px';
          } else {
            row.className = 'msg-row received';
            bubble.style.backgroundColor = getHslaColor(rowUid); 
            bubble.style.borderBottomLeftRadius = '4px';
          }
        });

        let userHash = JSON.stringify(currentUsers);
        if (userHash !== lastKnownUsers) {
           let sel = document.getElementById('targetUser');
           let prevVal = sel.value;
           sel.innerHTML = "<option value='all'>Everyone</option>";
           for (let id in currentUsers) sel.innerHTML += `<option value='${id}'>${currentUsers[id]}</option>`;
           if (currentUsers[prevVal] || prevVal === 'all') sel.value = prevVal; 
           lastKnownUsers = userHash;
        }
        
        if (c.innerHTML !== temp.innerHTML) {
          c.innerHTML = temp.innerHTML;
          if (isAtBottom) c.scrollTop = c.scrollHeight;
          if (knownMsgCount > 0 && filteredRowsCount > knownMsgCount) {
             let lastRow = c.lastElementChild;
             if (lastRow && lastRow.getAttribute('data-uid') !== uid) playBeep();
          }
          knownMsgCount = filteredRowsCount;
        }
      }).catch(()=>{});
    }
    setInterval(update, 2000);
    update();

    document.getElementById('form').onsubmit = (e) => {
      e.preventDefault();
      let msgField = document.getElementById('msg');
      let targetUser = document.getElementById('targetUser').value;
      let currentNick = nickField.value.trim() || defaultNick; 
      // Generate unique Message ID for reactions
      let msgId = Math.random().toString(36).substring(2, 10); 
      let body = new URLSearchParams({uid: uid, nick: currentNick, msg: msgField.value, target: targetUser, msgId: msgId});
      
      msgField.value = ''; 
      fetch('/send', {method: 'POST', body: body}).then(update).catch(()=>{});
    };
  </script>
</body>
</html>
)rawliteral";




void setup() {
  pixels.begin();
  pixels.clear();
  pixels.show();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
  dnsServer.start(DNS_PORT, "*", apIP);
server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(200, "text/html; charset=utf-8", index_html);
  });
server.on("/msgs", HTTP_GET, []() {
    server.send(200, "text/html; charset=utf-8", chatHistory);
  });



  server.on("/join", HTTP_POST, []() {
    if (server.hasArg("uid")) {
      String uid = server.arg("uid");
      if (knownUIDs.indexOf(uid) == -1) {
        knownUIDs += uid + ",";
        blinkLED(64, 0, 64);
      }
    }
    server.send(200, "text/plain", "OK");
  });


server.on("/send", HTTP_POST, []() {
    if (server.hasArg("msg") && server.hasArg("uid") && server.hasArg("msgId")) {
      String uid = server.arg("uid");
      String msgId = server.arg("msgId");
      String target = server.hasArg("target") ? server.arg("target") : "all";
      String nick = server.hasArg("nick") ? server.arg("nick") : "Anon";
      String newMsg = server.arg("msg");
      
      nick.replace("<", "&lt;"); nick.replace(">", "&gt;");
      newMsg.replace("<", "&lt;"); newMsg.replace(">", "&gt;");
      
      String privateTag = (target != "all") ? "<span class='private-tag'> [Private]</span>" : "";
      
      // We append the msgId and an empty .rxns div with a unique ID lock
      chatHistory += "<div class='msg-row' data-uid='" + uid + "' data-target='" + target + "' data-msgid='" + msgId + "'><div class='bubble'><div class='nick'>" + nick + privateTag + "</div>" + newMsg + "<div class='rxns' id='r-" + msgId + "'></div></div></div>";
      
      blinkLED(0, 64, 0); 
    }
    server.send(200, "text/plain", "OK");
  });

  // Handle Incoming Reactions (The Injection Hack)
  server.on("/react", HTTP_POST, []() {
    if (server.hasArg("id") && server.hasArg("e")) {
      String id = server.arg("id");
      String emoji = server.arg("e");
      
      // Locate the exact empty reaction div for this message and insert the span
      String targetLock = "id='r-" + id + "'>";
      String injectedEmoji = "id='r-" + id + "'><span>" + emoji + "</span>";
      
      chatHistory.replace(targetLock, injectedEmoji);
    }
    server.send(200, "text/plain", "OK");
  });

  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  server.begin();
}



void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
