import axios from "axios";

const localURL = "http://localhost:5000";

const renderURL = "https://aiair-server.onrender.com";

//------------Temperature----------------
export const predictTempWithProphetLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet-lstm/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/temp", {
    dataTemp: data,
  });
  return response;
};

export const predictTempWithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/temp", {
    dataTemp: data,
  });
  return response;
};

//------------Humidity----------------
export const predictHumiWithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/humi", {
    dataHumi: data,
  });
  return response;
};

export const predictHumiWithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/humi", {
    dataHumi: data,
  });
  return response;
};

export const predictHumiWithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/humi", {
    dataHumi: data,
  });
  return response;
};

export const predictHumiWithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/humi", {
    dataHumi: data,
  });
  return response;
};

export const predictHumiWithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/humi", {
    dataHumi: data,
  });
  return response;
};

export const predictHumiWithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/humi", {
    dataHumi: data,
  });
  return response;
};

//------------CO2----------------
export const predictCO2WithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/co2", {
    dataCO2: data,
  });
  return response;
};

export const predictCO2WithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/co2", {
    dataCO2: data,
  });
  return response;
};

export const predictCO2WithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/co2", {
    dataCO2: data,
  });
  return response;
};

export const predictCO2WithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/co2", {
    dataCO2: data,
  });
  return response;
};

export const predictCO2WithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/co2", {
    dataCO2: data,
  });
  return response;
};

export const predictCO2WithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/co2", {
    dataCO2: data,
  });
  return response;
};

//------------CO----------------
export const predictCOWithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/co", {
    dataCO: data,
  });
  return response;
};

export const predictCOWithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/co", {
    dataCO: data,
  });
  return response;
};

export const predictCOWithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/co", {
    dataCO: data,
  });
  return response;
};

export const predictCOWithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/co", {
    dataCO: data,
  });
  return response;
};

export const predictCOWithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/co", {
    dataCO: data,
  });
  return response;
};

export const predictCOWithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/co", {
    dataCO: data,
  });
  return response;
};

//------------UV----------------
export const predictUVWithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/uv", {
    dataUV: data,
  });
  return response;
};

export const predictUVWithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/uv", {
    dataUV: data,
  });
  return response;
};

export const predictUVWithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/uv", {
    dataUV: data,
  });
  return response;
};

export const predictUVWithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/uv", {
    dataUV: data,
  });
  return response;
};

export const predictUVWithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/uv", {
    dataUV: data,
  });
  return response;
};

export const predictUVWithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/uv", {
    dataUV: data,
  });
  return response;
};

//------------PM2.5----------------
export const predictPM25WithProphet = async (data) => {
  const response = await axios.post(localURL + "/predict/prophet/pm25", {
    dataPM25: data,
  });
  return response;
};

export const predictPM25WithLSTM = async (data) => {
  const response = await axios.post(localURL + "/predict/lstm/pm25", {
    dataPM25: data,
  });
  return response;
};

export const predictPM25WithLR = async (data) => {
  const response = await axios.post(localURL + "/predict/lr/pm25", {
    dataPM25: data,
  });
  return response;
};

export const predictPM25WithGB = async (data) => {
  const response = await axios.post(localURL + "/predict/gb/pm25", {
    dataPM25: data,
  });
  return response;
};

export const predictPM25WithXGB = async (data) => {
  const response = await axios.post(localURL + "/predict/xgb/pm25", {
    dataPM25: data,
  });
  return response;
};

export const predictPM25WithRF = async (data) => {
  const response = await axios.post(localURL + "/predict/rf/pm25", {
    dataPM25: data,
  });
  return response;
};
