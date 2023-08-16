import axios from "axios";

export const getNewestDataThingSpeak = async () => {
  const response = await axios.get(
    "https://api.thingspeak.com/channels/2115707/feeds.json?results=1"
  );
  return response.data;
};

export const getDataOfCOThingSpeak = async (start, end) => {
  const response = await axios.get(
    `https://api.thingspeak.com/channels/2115707/fields/4.json?timezone=Asia%2Bangkok&results=288&start=${start}&end=${end}`
  );
  return response;
};

export const predictCO = async (data) => {
  const response = await axios.post("http://localhost:5000/predict/co", {
    dataCO: data,
  });
  return response;
};
