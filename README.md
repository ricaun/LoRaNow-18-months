# LoRaNow 18 Months

This project is a long run using the [LoRaNow Board V0](https://github.com/ricaun/LoRaNow-board) and [The Things Network](https://www.thethingsnetwork.org/) to check how long a node can stay on only with two AA battery.

The Node starts in April of 2019 and dies in September of 2020, 18 months in total.

The Node sleeps and send data using LoRaWAN each ~10 minutes.

# Data

The data uses a JSON format and has 3 files with the data group by days, hours, and minutes.

```
[
  {
    "date": "2020-09-29 12:00:00",
    "gateway_rssi": "-45",
    "counter": "71086",
    "counterMin": "70987",
    "vccMin": "2.15",
    "vcc": "2.23",
    "vccMax": "2.26",
    "tempMin": "22.00",
    "temp": "22.96",
    "tempMax": "24.10",
    "barMin": "1010.40",
    "bar": "1012.02",
    "barMax": "1014.20"
  },
  ...
]
```

* [data-days.json](data/data-days.json)
* [data-hours.json](data/data-hours.json)
* [data-minutes.json](data/data-minutes.json)

## License

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.

----

See news and other projects on my [LoRaNow Blog](http://loranow.com)
